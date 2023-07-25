/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file land_outfits.c
 *
 * @brief Handles all the landing menus and actions.
 */


/** @cond */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "naev.h"
/** @endcond */

#include "land_outfits.h"

#include "array.h"
#include "dialogue.h"
#include "equipment.h"
#include "hook.h"
#include "land.h"
#include "log.h"
#include "map.h"
#include "map_find.h"
#include "nstring.h"
#include "outfit.h"
#include "player.h"
#include "player_gui.h"
#include "slots.h"
#include "space.h"
#include "tk/toolkit_priv.h"
#include "toolkit.h"


#define  OUTFITS_IAR    "iarOutfits"
#define  OUTFITS_TAB    "tabOutfits"
#define  OUTFITS_FILTER "inpFilterOutfits"
#define  OUTFITS_NTABS  6


typedef struct LandOutfitData_ {
   const Outfit **outfits;
} LandOutfitData;


static iar_data_t *iar_data = NULL; /**< Stored image array positions. */
static Outfit ***iar_outfits = NULL; /**< C-array of Arrays: Outfits associated with the image array cells. */

/* Modifier for buying and selling quantity. */
static int outfits_mod = 1;


/*
 * Helper functions.
 */
static void outfits_getSize( unsigned int wid, int *w, int *h,
      int *iw, int *ih, int *bw, int *bh );
static void outfits_buy( unsigned int wid, char* str );
static void outfits_sell( unsigned int wid, char* str );
static int outfits_getMod (void);
static void outfits_renderMod( double bx, double by, double w, double h, void *data );
static void outfits_rmouse( unsigned int wid, char* widget_name );
static void outfits_find( unsigned int wid, char* str );
static credits_t outfit_getPrice( Outfit *outfit );
static void outfits_genList( unsigned int wid );
static void outfits_changeTab( unsigned int wid, char *wgt, int old, int tab );
static void outfits_onClose( unsigned int wid, char *str );


/**
 * @brief Gets the size of the outfits window.
 */
static void outfits_getSize( unsigned int wid, int *w, int *h,
      int *iw, int *ih, int *bw, int *bh )
{
   int padding;

   /* Get window dimensions. */
   window_dimWindow( wid, w, h );

   /* Calculate image array dimensions. */
   if (iw != NULL)
      *iw = 704 + (*w - LAND_WIDTH);
   if (ih != NULL)
      *ih = *h - 60;

   /* Left padding + per-button padding * nbuttons */
   padding = 20 + 10*5;

   /* Calculate button dimensions. */
   if (bw != NULL)
      *bw = (*w - (iw!=NULL?*iw:0) - padding) / 4;
   if (bh != NULL)
      *bh = LAND_BUTTON_HEIGHT;
}


/**
 * @brief For when the widget closes.
 */
static void outfits_onClose( unsigned int wid, char *str )
{
   (void) str;
   LandOutfitData *data = window_getData( wid );
   if (data==NULL)
      return;
   array_free( data->outfits );
   free( data );
}



/**
 * @brief Opens the outfit exchange center window.
 *
 *    @param wid Window ID to open at.
 *    @param outfits Array (array.h): Outfits to sell. Will be freed.
 *                   Set to NULL if this is the landed player store.
 */
void outfits_open( unsigned int wid, const Outfit **outfits )
{
   int w, h, iw, ih, bw, bh, off;
   LandOutfitData *data = NULL;

   /* Set up window data. */
   if (outfits!=NULL) {
      data           = malloc( sizeof( LandOutfitData ) );
      data->outfits  = outfits;
      window_setData( wid, data );
      window_onClose( wid, outfits_onClose );
   }

   /* Mark as generated. */
   if (outfits==NULL)
      land_tabGenerate(LAND_WINDOW_OUTFITS);

   /* Get dimensions. */
   outfits_getSize( wid, &w, &h, &iw, &ih, &bw, &bh );

   /* Initialize stored positions. */
   if (iar_data == NULL)
      iar_data = calloc( OUTFITS_NTABS, sizeof(iar_data_t) );
   else
      memset( iar_data, 0, sizeof(iar_data_t) * OUTFITS_NTABS );
   if (iar_outfits == NULL)
      iar_outfits = calloc( OUTFITS_NTABS, sizeof(Outfit**) );
   else {
      for (int i=0; i<OUTFITS_NTABS; i++)
         array_free( iar_outfits[i] );
      memset( iar_outfits, 0, sizeof(Outfit**) * OUTFITS_NTABS );
   }

   /* will allow buying from keyboard */
   window_setAccept( wid, outfits_buy );

   /* buttons */
   if (data==NULL) {
      window_addButtonKey(wid, off = -10, 20,
            bw, bh, "btnCloseOutfits",
            _("&Take Off"), land_buttonTakeoff, SDLK_t);
   }
   else {
      window_addButtonKey(wid, off = -10, 20,
            bw, bh, "btnCloseOutfits",
            _("Close"), window_close, SDLK_t);
   }
   window_addButtonKey(wid, off -= 10+bw, 20,
         bw, bh, "btnSellOutfit",
         _("&Sell"), outfits_sell, SDLK_s);
   window_addButtonKey(wid, off -= 10+bw, 20,
         bw, bh, "btnBuyOutfit",
         _("&Buy"), outfits_buy, SDLK_b);
   window_addButtonKey(wid, off -= 10+bw, 20,
         bw, bh, "btnFindOutfits",
         _("&Find Outfits"), outfits_find, SDLK_f);
   (void)off;

   /* fancy 192x192 image */
   window_addRect(wid, -17, -16, 200, 199, "rctImage", &cBlack, 0);
   window_addImage(wid, -20, -20, 192, 192, "imgOutfit", NULL, 1);

   /* the descriptive text */
   window_addText(wid, 20 + iw + 20, -40, w - (20+iw+20) - 200 - 20, 160, 0,
         "txtOutfitName", &gl_defFont, NULL, NULL);

   window_addText(wid, 20 + iw + 20, 0, w - (20+iw+20) - 200 - 20, 160, 0,
         "txtDDesc", &gl_defFont, NULL, NULL);
   window_addText(wid, 20 + iw + 20, 0, w - (20+iw+20) - 20, 320, 0,
         "txtDescShort", &gl_defFont, NULL, NULL);
   window_addText(wid, 20 + iw + 20, 0, w - (20+iw+20) - 20, 160, 0,
         "txtDescription", &gl_smallFont, NULL, NULL);

   /* cust draws the modifier */
   window_addCust(wid, -20 - (20+bw) - (10+bw) + 20, 20+bh+10,
         40, 2*gl_smallFont.h, "cstMod", 0, outfits_renderMod, NULL, NULL);

   /* Create the image array. */
   outfits_genList( wid );

   /* Set default keyboard focus to the list */
   window_setFocus( wid , OUTFITS_IAR );
}


/**
 * @brief Regenerates the outfit list.
 *
 *   @param wid Window to generate the list on.
 *   @param str Unused.
 */
void outfits_regenList( unsigned int wid, char *str )
{
   (void) str;
   int tab;
   char *focused;
   LandOutfitData *data;

   /* If local or not. */
   data = window_getData( wid );

   /* Must exist. */
   if ((data==NULL) && (land_getWid( LAND_WINDOW_OUTFITS ) == 0))
      return;

   /* Save focus. */
   focused = window_getFocus( wid );

   /* Save positions. */
   tab = window_tabWinGetActive( wid, OUTFITS_TAB );
   toolkit_saveImageArrayData( wid, OUTFITS_IAR, &iar_data[tab] );
   window_destroyWidget( wid, OUTFITS_IAR );

   outfits_genList( wid );

   /* Restore positions. */
   toolkit_setImageArrayPos(    wid, OUTFITS_IAR, iar_data[tab].pos );
   toolkit_setImageArrayOffset( wid, OUTFITS_IAR, iar_data[tab].offset );
   outfits_update( wid, NULL );

   /* Restore focus. */
   window_setFocus( wid, focused );
   free(focused);
}

/**
 * @brief Generates the outfit list.
 *
 *    @param wid Window to generate the list on.
 */
static void outfits_genList( unsigned int wid )
{
   int (*tabfilters[])( const Outfit *o ) = {
      NULL,
      outfit_filterWeapon,
      outfit_filterUtility,
      outfit_filterStructure,
      outfit_filterCore,
      outfit_filterOther
   };
   const char *tabnames[] = {
      OUTFIT_LABEL_ALL, OUTFIT_LABEL_WEAPON, OUTFIT_LABEL_UTILITY,
      OUTFIT_LABEL_STRUCTURE, OUTFIT_LABEL_CORE, OUTFIT_LABEL_OTHER
   };

   int active;
   int fx, fy, fw, fh, barw; /* Input filter. */
   ImageArrayCell *coutfits;
   int noutfits;
   int w, h, iw, ih;
   char *filtertext;
   LandOutfitData *data;
   int iconsize;

   /* Get dimensions. */
   outfits_getSize( wid, &w, &h, &iw, &ih, NULL, NULL );

   /* Create tabbed window. */
   if (!widget_exists( wid, OUTFITS_TAB )) {
      window_addTabbedWindow( wid, 20, 20 + ih - 30, iw, 30,
         OUTFITS_TAB, OUTFITS_NTABS, tabnames, 1 );

      barw = window_tabWinGetBarWidth( wid, OUTFITS_TAB );
      fw = CLAMP(0, 150, iw - barw - 30);
      fh = 20;

      fx = iw - fw;
      fy = ih - (30 - fh) / 2; /* Centered relative to 30 px tab bar */

      /* Only create the filter widget if it will be a reasonable size. */
      if (iw >= 30) {
         window_addInput( wid, fx + 15, fy +1, fw, fh, OUTFITS_FILTER, 32, 1, &gl_smallFont );
         inp_setEmptyText( wid, OUTFITS_FILTER, _("Filter…") );
         window_setInputCallback( wid, OUTFITS_FILTER, outfits_regenList );
      }
   }

   window_tabWinOnChange( wid, OUTFITS_TAB, outfits_changeTab );
   active = window_tabWinGetActive( wid, OUTFITS_TAB );

   /* Widget must not already exist. */
   if (widget_exists( wid, OUTFITS_IAR ))
      return;

   filtertext = NULL;
   if (widget_exists( wid, OUTFITS_FILTER )) {
      filtertext = window_getInput( wid, OUTFITS_FILTER );
      if (strlen(filtertext) == 0)
         filtertext = NULL;
   }

   /* Set up the outfits to buy/sell */
   data = window_getData( wid );
   array_free( iar_outfits[active] );
   /* Use custom list; default to landed outfits. */
   iar_outfits[active] = data!=NULL ? array_copy( Outfit*, data->outfits ) : tech_getOutfit( land_planet->tech );
   noutfits = outfits_filter( (const Outfit**)iar_outfits[active], array_size(iar_outfits[active]), tabfilters[active], filtertext );
   coutfits = outfits_imageArrayCells( (const Outfit**)iar_outfits[active], &noutfits );

   iconsize = 128;
   window_addImageArray( wid, 20, 20,
         iw, ih - 34, OUTFITS_IAR, iconsize, iconsize,
         coutfits, noutfits, outfits_update, outfits_rmouse, NULL );

   /* write the outfits stuff */
   outfits_update( wid, NULL );
}


/**
 * @brief Updates the outfits in the outfit window.
 *    @param wid Window to update the outfits in.
 *    @param str Unused.
 */
void outfits_update( unsigned int wid, char* str )
{
   (void)str;
   int i, active;
   Outfit* outfit;
   char buf[STRMAX], buf_price[ECON_CRED_STRLEN],
         buf_credits[ECON_CRED_STRLEN], buf_license[STRMAX_SHORT],
         buf_mass[STRMAX_SHORT];
   int y, tw, th;
   int w, h, iw, ih, bh;

   /* Get dimensions. */
   outfits_getSize(wid, &w, &h, &iw, &ih, NULL, &bh);

   /* Get and set parameters. */
   active = window_tabWinGetActive( wid, OUTFITS_TAB );
   i = toolkit_getImageArrayPos( wid, OUTFITS_IAR );
   if (i < 0 || array_size(iar_outfits[active]) == 0) { /* No outfits */
      window_modifyImage(wid, "imgOutfit", NULL, 192, 192);
      window_disableButton(wid, "btnBuyOutfit");
      window_disableButton(wid, "btnSellOutfit");
      window_modifyText(wid, "txtOutfitName", _("None"));
      window_modifyText(wid, "txtDDesc", NULL);
      window_modifyText(wid, "txtDescShort", NULL);
      window_modifyText(wid, "txtDescription", NULL);
      return;
   }

   outfit = iar_outfits[active][i];

   /* new image */
   window_modifyImage( wid, "imgOutfit", outfit->gfx_store, 192, 192 );

   if (outfit_canBuy(outfit->name, land_planet) > 0)
      window_enableButton( wid, "btnBuyOutfit" );
   else
      window_disableButtonSoft( wid, "btnBuyOutfit" );

   /* gray out sell button */
   if (outfit_canSell(outfit->name) > 0)
      window_enableButton( wid, "btnSellOutfit" );
   else
      window_disableButtonSoft( wid, "btnSellOutfit" );

   /* new text */
   price2str( buf_price, outfit_getPrice(outfit), player.p->credits, 2 );
   credits2str( buf_credits, player.p->credits, 2 );

   if (outfit->license == NULL)
      strncpy(buf_license, _("None"), sizeof(buf_license)-1);
   else if (player_hasLicense(outfit->license)
         || (land_planet != NULL
               && planet_hasService(land_planet, PLANET_SERVICE_BLACKMARKET)))
      strncpy(buf_license, _(outfit->license), sizeof(buf_license)-1);
   else
      snprintf(buf_license, sizeof(buf_license), "#r%s#0", _(outfit->license));

   if ((outfit_isLauncher(outfit) || outfit_isFighterBay(outfit)) &&
         (outfit_ammo(outfit) != NULL)) {
      /* Translation note: this is a range from one mass to another,
       * indicating minimum and maximum mass of a launcher or fighter
       * bay. */
      snprintf(buf_mass, sizeof(buf_mass), _("%.0f–%.0f kt"),
            outfit->mass,
            outfit->mass + outfit_amount(outfit)*outfit_ammo(outfit)->mass);
   }
   else {
      snprintf(buf_mass, sizeof(buf_mass), _("%.0f kt"), outfit->mass);
   }

   window_modifyText(wid, "txtOutfitName", _(outfit->name));
   window_dimWidget(wid, "txtOutfitName", &tw, &th);
   th = gl_printHeightRaw(&gl_defFont, tw, _(outfit->name));
   y = -40 - th - 30;

   snprintf(buf, sizeof(buf),
         _("#nSlot:#0 %s (%s)\n"
            "#nMass:#0 %s\n"
            "#nPrice:#0 %s\n"
            "#nMoney:#0 %s\n"
            "#nLicense:#0 %s"),
         (outfit->slot.spid == 0) ?
            _(outfit_slotName(outfit)) : _(sp_display(outfit->slot.spid)),
         _(outfit_slotSize(outfit)),
         buf_mass,
         buf_price,
         buf_credits,
         buf_license);
   window_modifyText(wid, "txtDDesc", buf);
   window_dimWidget(wid, "txtDDesc", &tw, &th);
   th = gl_printHeightRaw(&gl_defFont, tw, buf);
   window_resizeWidget(wid, "txtDDesc", tw, th);
   window_moveWidget(wid, "txtDDesc", 20+iw+20, y);
   y -= th + 20;

   window_modifyText(wid, "txtDescShort", outfit->desc_short);
   window_dimWidget(wid, "txtDescShort", &tw, &th);
   th = gl_printHeightRaw(&gl_defFont, tw, outfit->desc_short);
   window_resizeWidget(wid, "txtDescShort", tw, th);
   window_moveWidget(wid, "txtDescShort", 20+iw+20, y);
   y -= th + 20;

   window_modifyText(wid, "txtDescription", _(outfit->description));
   window_dimWidget(wid, "txtDescription", &tw, &th);
   th = h + y - bh - 20;
   window_resizeWidget(wid, "txtDescription", tw, th);
   window_moveWidget(wid, "txtDescription", 20+iw+20, y);
}


/**
 * @brief Updates the outfitter and equipment outfit image arrays.
 */
void outfits_updateEquipmentOutfits( void )
{
   int ew, ow;

   if (landed && land_doneLoading()) {
      if (planet_hasService(land_planet, PLANET_SERVICE_OUTFITS)) {
         ow = land_getWid( LAND_WINDOW_OUTFITS );
         outfits_regenList( ow, NULL );
      }
      else if (!planet_hasService(land_planet, PLANET_SERVICE_SHIPYARD))
         return;
      ew = land_getWid( LAND_WINDOW_EQUIPMENT );
      equipment_addAmmo();
      equipment_regenLists( ew, 1, 0 );
   }
}


/**
 * @brief Ensures the tab's selected item is reflected in the ship slot list
 *
 *    @param wid Unused.
 *    @param wgt Unused.
 *    @param old Tab changed from.
 *    @param tab Tab changed to.
 */
static void outfits_changeTab( unsigned int wid, char *wgt, int old, int tab )
{
   (void) wid;
   (void) wgt;
   int pos;
   double offset;

   toolkit_saveImageArrayData( wid, OUTFITS_IAR, &iar_data[old] );

   /* Store the currently-saved positions for the new tab. */
   pos    = iar_data[tab].pos;
   offset = iar_data[tab].offset;

   /* Resetting the input will cause the outfit list to be regenerated. */
   if (widget_exists(wid, OUTFITS_FILTER))
      window_setInput(wid, OUTFITS_FILTER, NULL);
   else
      outfits_regenList( wid, NULL );

   /* Set positions for the new tab. This is necessary because the stored
    * position for the new tab may have exceeded the size of the old tab,
    * resulting in it being clipped. */
   toolkit_setImageArrayPos(    wid, OUTFITS_IAR, pos );
   toolkit_setImageArrayOffset( wid, OUTFITS_IAR, offset );

   /* Focus the outfit image array. */
   window_setFocus( wid, OUTFITS_IAR );
}


/**
 * @brief Applies a filter function and string to a list of outfits.
 *
 *    @param outfits Array of outfits to filter.
 *    @param n Number of outfits in the array.
 *    @param filter Filter function to run on each outfit.
 *    @param name Name fragment that each outfit name must contain.
 *    @return Number of outfits.
 */
int outfits_filter( const Outfit **outfits, int n,
      int(*filter)( const Outfit *o ), char *name )
{
   int i, j;

   j = 0;
   for (i=0; i<n; i++) {
      if ((filter != NULL) && !filter(outfits[i]))
         continue;

      if ((name != NULL) && (strcasestr(_(outfits[i]->name), name) == NULL))
         continue;

      /* Shift matches downward. */
      outfits[j] = outfits[i];
      j++;
   }

   return j;
}


/**
 * @brief Starts the map find with outfit search selected.
 *    @param wid Window buying outfit from.
 *    @param str Unused.
 */
static void outfits_find( unsigned int wid, char* str )
{
   (void) str;
   map_inputFindType(wid, "outfit");
}


/**
 * @brief Returns the price of an outfit (subject to quantity modifier)
 */
static credits_t outfit_getPrice( Outfit *outfit )
{
   unsigned int q;
   credits_t price;

   q = outfits_getMod();
   price = outfit->price * q;

   return price;
}


/**
 * @brief Computes the alt text for an outfit.
 */
int outfit_altText( char *buf, int n, const Outfit *o )
{
   int p;
   double mass;
   char buf_price[ECON_CRED_STRLEN];
   char buf_mass[STRMAX_SHORT];

   credits2str(buf_price, o->price, 2);

   mass = o->mass;
   if ((outfit_isLauncher(o) || outfit_isFighterBay(o)) &&
         (outfit_ammo(o) != NULL)) {
      mass += outfit_amount(o) * outfit_ammo(o)->mass;
      /* Translation note: this is a range from one mass to another,
       * indicating minimum and maximum mass of a launcher or fighter
       * bay. */
      snprintf(buf_mass, sizeof(buf_mass), _("%.0f–%.0f kt"),
            o->mass, mass);
   }
   else {
      snprintf(buf_mass, sizeof(buf_mass), _("%.0f kt"), mass);
   }

   p  = scnprintf(&buf[0], n, "%s\n", _(o->name));
   if (o->slot.type != OUTFIT_SLOT_NA) {
      p += scnprintf(&buf[p], n-p, _("Requires %s slot (%s)"),
            (o->slot.spid == 0) ?
               _(outfit_slotName(o)) : _(sp_display(o->slot.spid)),
            outfit_slotSize(o));
      p += scnprintf(&buf[p], n-p, "\n");
   }
   if ((o->mass > 0.) && (p < n))
      p += scnprintf(&buf[p], n-p, "%s\n", buf_mass);
   if (o->price > 0.)
      p += scnprintf(&buf[p], n-p, "%s\n", buf_price);
   if (outfit_isProp(o, OUTFIT_PROP_UNIQUE))
      p += scnprintf(&buf[p], n-p, "#o%s#0\n", p_("outfit", "Unique"));

   p += scnprintf( &buf[p], n-p, "\n%s", o->desc_short );

   return 0;
}


/**
 * @brief Generates image array cells corresponding to outfits.
 */
ImageArrayCell *outfits_imageArrayCells( const Outfit **outfits, int *noutfits )
{
   int i;
   const glColour *c;
   ImageArrayCell *coutfits;
   const Outfit *o;
   const char *typename;
   glTexture *t;
   size_t n;

   /* Allocate. */
   coutfits = calloc( MAX(1,*noutfits), sizeof(ImageArrayCell) );

   if (*noutfits == 0) {
      *noutfits = 1;
      coutfits[0].image = NULL;
      coutfits[0].caption = strdup( _("None") );
   }
   else {
      /* Set alt text. */
      for (i=0; i<*noutfits; i++) {
         o = outfits[i];

         coutfits[i].image = gl_dupTexture( o->gfx_store );
         coutfits[i].caption = strdup( _(o->name) );
         coutfits[i].quantity = player_outfitOwned(o);

         /* Background colour. */
         c = outfit_slotSizeColour( &o->slot );
         if (c == NULL)
            c = &cBlack;
         col_blend( &coutfits[i].bg, c, &cGrey70, 1 );

         /* Short description. */
         if (o->desc_short == NULL)
            coutfits[i].alt = NULL;
         else {
            coutfits[i].alt = malloc( STRMAX );
            outfit_altText( coutfits[i].alt, STRMAX, o );
         }

         /* Slot type (outfit size). */
         switch (o->slot.size) {
            case OUTFIT_SLOT_SIZE_LIGHT:
               /* Abbreviation for "Small"; must be only one character. */
               typename = p_("outfit_size", "S");
               break;
            case OUTFIT_SLOT_SIZE_MEDIUM:
               /* Abbreviation for "Medium"; must be only one character. */
               typename = p_("outfit_size", "M");
               break;
            case OUTFIT_SLOT_SIZE_HEAVY:
               /* Abbreviation for "Large"; must be only one character. */
               typename = p_("outfit_size", "L");
               break;
            default:
               typename = NULL;
         }
         if (typename != NULL) {
            /* Add one for the terminating null character. */
            n = strlen(typename) + 1;
            coutfits[i].slottype = malloc(n);
            strcpy(coutfits[i].slottype, typename);
         }

         /* Layers. */
         coutfits[i].layers = gl_copyTexArray( o->gfx_overlays, &coutfits[i].nlayers );
         if (o->rarity > 0) {
            t = rarity_texture( o->rarity );
            coutfits[i].layers = gl_addTexArray( coutfits[i].layers, &coutfits[i].nlayers, t );
         }
      }
   }
   return coutfits;
}



/**
 * @brief Checks to see if the player can buy the outfit.
 *    @param name Outfit to buy.
 *    @param planet Where the player is shopping.
 */
int outfit_canBuy( const char *name, Planet *planet )
{
   int failure;
   credits_t price;
   Outfit *outfit;
   char buf[ECON_CRED_STRLEN];

   failure = 0;
   outfit  = outfit_get(name);
   price   = outfit_getPrice(outfit);

   /* Unique. */
   if (outfit_isProp(outfit, OUTFIT_PROP_UNIQUE) && (player_outfitOwnedTotal(outfit)>0)) {
      land_errDialogueBuild( _("You can only own one of this outfit.") );
      return 0;
   }

   /* Map already mapped */
   if ((outfit_isMap(outfit) && map_isUseless(outfit)) ||
         (outfit_isLocalMap(outfit) && localmap_isUseless())) {
      land_errDialogueBuild( _("You already know of everything this map contains.") );
      return 0;
   }
   /* Already has license. */
   if (outfit_isLicense(outfit) && player_hasLicense(outfit->name)) {
      land_errDialogueBuild( _("You already have this license.") );
      return 0;
   }
   /* not enough $$ */
   if (!player_hasCredits(price)) {
      credits2str( buf, price - player.p->credits, 2 );
      land_errDialogueBuild( _("You need %s more."), buf);
      failure = 1;
   }
   /* Needs license. */
   if ((!player_hasLicense(outfit->license)) &&
         ((planet == NULL) || (!planet_hasService(planet, PLANET_SERVICE_BLACKMARKET)))) {
      land_errDialogueBuild( _("License needed: %s."),
               _(outfit->license) );
      failure = 1;
   }

   return !failure;
}


/**
 * @brief Player right-clicks on an outfit.
 *    @param wid Window player is buying ship from.
 *    @param widget_name Name of the window. (unused)
 */
static void outfits_rmouse( unsigned int wid, char* widget_name )
{
    outfits_buy( wid, widget_name );
}

/**
 * @brief Attempts to buy the outfit that is selected.
 *    @param wid Window buying outfit from.
 *    @param str Unused.
 */
static void outfits_buy( unsigned int wid, char* str )
{
   (void) str;
   int i, active;
   Outfit* outfit;
   int q;
   HookParam hparam[3];

   active = window_tabWinGetActive( wid, OUTFITS_TAB );
   i = toolkit_getImageArrayPos( wid, OUTFITS_IAR );
   if (i < 0 || array_size(iar_outfits[active]) == 0)
      return;

   outfit = iar_outfits[active][i];
   q = outfits_getMod();
   /* Can only get one unique item. */
   if (outfit_isProp(outfit, OUTFIT_PROP_UNIQUE)
         || outfit_isMap(outfit) || outfit_isLocalMap(outfit)
         || outfit_isLicense(outfit))
      q = MIN(q,1);

   /* can buy the outfit? */
   if (land_errDialogue( outfit->name, "buyOutfit" ))
      return;

   /* Actually buy the outfit. */
   player_modCredits( -outfit->price * player_addOutfit( outfit, q ) );
   outfits_updateEquipmentOutfits();
   hparam[0].type    = HOOK_PARAM_STRING;
   hparam[0].u.str   = outfit->name;
   hparam[1].type    = HOOK_PARAM_NUMBER;
   hparam[1].u.num   = q;
   hparam[2].type    = HOOK_PARAM_SENTINEL;
   hooks_runParam( "outfit_buy", hparam );
   if (land_takeoff)
      takeoff(1);

   /* Regenerate list. */
   outfits_regenList( wid, NULL );
}
/**
 * @brief Checks to see if the player can sell the selected outfit.
 *    @param name Outfit to try to sell.
 */
int outfit_canSell( const char *name )
{
   int failure;
   Outfit *outfit;

   failure = 0;
   outfit = outfit_get(name);

   /* Unique item. */
   if (outfit_isProp(outfit, OUTFIT_PROP_UNIQUE)) {
      land_errDialogueBuild(_("You can't sell a unique outfit."));
      failure = 1;
   }

   /* Map check. */
   if (outfit_isMap(outfit) || outfit_isLocalMap(outfit)) {
      land_errDialogueBuild(_("You can't sell a map."));
      failure = 1;
   }

   /* License check. */
   if (outfit_isLicense(outfit)) {
      land_errDialogueBuild(_("You can't sell a license."));
      failure = 1;
   }

   /* has no outfits to sell */
   if (player_outfitOwned(outfit) <= 0) {
      land_errDialogueBuild( _("You can't sell something you don't have!") );
      failure = 1;
   }

   return !failure;
}
/**
 * @brief Attempts to sell the selected outfit the player has.
 *    @param wid Window selling outfits from.
 *    @param str Unused.
 */
static void outfits_sell( unsigned int wid, char* str )
{
   (void)str;
   int i, active;
   Outfit* outfit;
   int q;
   HookParam hparam[3];

   active = window_tabWinGetActive( wid, OUTFITS_TAB );
   i = toolkit_getImageArrayPos( wid, OUTFITS_IAR );
   if (i < 0 || array_size(iar_outfits[active]) == 0)
      return;

   outfit      = iar_outfits[active][i];

   q = outfits_getMod();

   /* Check various failure conditions. */
   if (land_errDialogue( outfit->name, "sellOutfit" ))
      return;

   player_modCredits( outfit->price * player_rmOutfit( outfit, q ) );
   outfits_updateEquipmentOutfits();
   hparam[0].type    = HOOK_PARAM_STRING;
   hparam[0].u.str   = outfit->name;
   hparam[1].type    = HOOK_PARAM_NUMBER;
   hparam[1].u.num   = q;
   hparam[2].type    = HOOK_PARAM_SENTINEL;
   hooks_runParam( "outfit_sell", hparam );
   if (land_takeoff)
      takeoff(1);

   /* Regenerate list. */
   outfits_regenList( wid, NULL );
}
/**
 * @brief Gets the current modifier status.
 *    @return The amount modifier when buying or selling outfits.
 */
static int outfits_getMod (void)
{
   SDL_Keymod mods;
   int q;

   mods = SDL_GetModState();
   q = 1;
   if (mods & (KMOD_LCTRL | KMOD_RCTRL))
      q *= 5;
   if (mods & (KMOD_LSHIFT | KMOD_RSHIFT))
      q *= 10;

   return q;
}
/**
 * @brief Renders the outfit buying modifier.
 *    @param bx Base X position to render at.
 *    @param by Base Y position to render at.
 *    @param w Width to render at.
 *    @param h Height to render at.
 *    @param data Unused.
 */
static void outfits_renderMod( double bx, double by, double w, double h, void *data )
{
   (void) data;
   (void) h;
   int tw, th;
   int q;
   char buf[STRMAX_SHORT];
   glColour c;
   const int off = 4;

   q = outfits_getMod();
   if (q != outfits_mod) {
      outfits_updateEquipmentOutfits();
      outfits_mod = q;
   }
   if (q==1) return; /* Ignore no modifier. */

   /* It's possible for there to be text underneath. */
   glClear(GL_DEPTH_BUFFER_BIT);

   snprintf(buf, 8, "%d×", q);

   by += off;
   tw = gl_printWidthRaw(&gl_smallFont, buf);
   th = gl_printHeightRaw(&gl_smallFont, tw, buf);
   c.r = toolkit_col->r;
   c.g = toolkit_col->g;
   c.b = toolkit_col->b;
   c.a = 0.95;

   gl_renderRect(bx + w/2 - tw/2 - off, by - off, tw + 2*off, th + 2*off, &c);
   gl_printMidRaw(&gl_smallFont, w, bx, by, &cFontWhite, -1, buf);
}


/**
 * @brief Cleans up outfit globals.
 */
void outfits_cleanup(void)
{
   /* Free stored positions. */
   free(iar_data);
   iar_data = NULL;
   if (iar_outfits != NULL) {
      for (int i=0; i<OUTFITS_NTABS; i++)
         array_free( iar_outfits[i] );
      free(iar_outfits);
      iar_outfits = NULL;
   }
}
