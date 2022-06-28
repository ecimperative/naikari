# NAIKARI CHANGELOG

## 0.4.0

* Corrected the calculation for beam heat-up; the previous inaccuracy
  led to beams heating up faster than they were supposed to.
* Adjusted the Equipment screen and the Info ship tab, showing the
  player's credits on the Equipment screen and the ship's value on the
  Info ship tab.
* Added a Net Worth stat to the Info overview tab, showing the total
  value of your credits, ships, and outfits combined.
* Added some NPC messages.
* Fixed and adjusted some missions.
* Knowledge of the FLF's hidden jumps is now erased if you betray them.
* Fixed bad rendering of marker text.
* The FLF/Dvaered derelicts event now requires a Mercenary License and
  can only occur outside of Frontier space.
* Changed the name of the Info Window to Ship Computer.

## 0.3.0

* Changed Soromid taunts (the old ones inherited from Naev sounded too
  much like social darwinism, a harmful and pseudo-scientific ideology).
* Changed volume sliders to show a percentage in a standard form rather
  than the raw floating-point numbers that were previously displayed.
* The alt text when hovering your mouse over an outfit has been
  slightly rearranged and a credits display has been added to it.
* Removed the once-per-version "Welcome to Naikari" message.
* Pirate names are now generated in a different way which should be a
  bit nicer.
* Added many more possibilities for the randomly generated pilot names.
* Fixed a bug which caused pirate cargo missions to incorrectly
  calculate number of jumps to the destination (leading to lower
  rewards and a warning about you not knowing the fastest route).
* Fixed a cosmetic bug which could lead to you having phantom cargo
  after stealing cargo from ships (caused by a condition that would lead
  to AI pilots having 0 tonnes of a cargo added to them).
* Bombers now have a 20% radar range bonus (which makes their missiles
  more accurate).
* Restructured the FLF campaign so that the "Anti-Dvaered" and "Hero"
  storylines happen in parallel, rather than in an entangled fashion.
* Tutorial has been streamlined so that it introduces the game much
  better than before.
* System names for unknown systems that are marked by a mission are now
  consistently displayed everywhere (whereas previously they were
  displayed in the starmap's sidebar, but nowhere else).
* Fixed some problems with the Waste Dump mission.
* Pulled new beam store graphics from Naev.
* Escort missions now have hardened claims, preventing edge cases of
  escort missions conflicting with other missions that do things like
  clear the system. A side effect of this is that trader escort missions
  are now only offered one at a time. In tandem with this, escort
  missions' probability has been adjusted so that they are still seen
  frequently, and the trader escort mission in particular has been given
  a higher priority so that bounty missions don't prevent it from
  showing up.
* Fixed glitchy text in overlay when appearing over other text.
* Fixed pirates sometimes using Lancelot fighter bays.
* Added several new texts for the Love Train mission.
* Changed the story context of the first FLF diversion mission; it is
  now a diversion to rescue FLF soldiers trapped in a Dvaered system.
* Added several new components for randomly generated pilot names.
* Mace rockets no longer have thrust, and rockets with thrust no longer
  start at 0 speed. This change was made because the calculations the
  game made for range were not even close when trying to account for the
  acceleration component (a problem that has existed for many years in
  Naev but only became a problem because of mace rockets' reduced
  range). Thrust is now used solely for guided missiles.
* Removed the planet named "Sheik Hall" since it has no gameplay value
  and its name is iffy.

## 0.2.1

* Fixed a bug that caused the game to not start under Windows.

## 0.2.0

* Changed the land and takeoff music to be the same as ambient music.
* Adjusted balancing of outfits and ships, most noticeably changing the
  Kestrel's two large fighter bay slots to medium fighter bay slots and
  making the Huntsman torpedo much stronger.
* All ships now come with pre-installed weapons when you buy them, not
  just your first ship.
* The warning shown when warping into a system with a volatile nebula
  now shows exactly how much damage your shield and armor take from it,
  rather than only showing the direct volatility rating.
* AI pilots now launch fighters if they have fighter bays.
* Several new fighter bays have been added (mostly miniaturized versions
  of existing fighter bays, but also three variations of a Shark
  fighter bay).
* Hired escorts that are created on restricted planets and stations now
  pilot factional military ships.
* The Info window's missions tab now displays the current objective
  according to the OSD.
* Logo now lights up red for Autism Acceptance Month, turns into a
  rainbow for Queer Pride Month, and turns into aromantic pride colors
  for Aromantic Spectrum Awareness Week.
* Asteroid Scanner now always shows you scan information for asteroids
  you can see, rather than only after you've targeted them.
* Replaced Improved Refrigeration Cycle with the Rotary Turbo Modulator,
  which does the opposite of what the Improved Refrigeration Cycle did
  and serves as an equivalent to the Power Regulation Override outfit
  for turrets.
* Instant mode weapons now show up on the weapon bar in the Brushed GUI.
* Ship Stealing mission now allows stealing of non-designated targets,
  at the cost of having to pay for the entire value of the ship (meaning
  it can't be used to make credits, it can only be used to steal
  particular ships you want to add to your collection). If it fails, it
  keeps running so you can steal a ship of your choice instead, and the
  OSD message changes to note this.
* Adjusted how much money ships carry to hopefully make illegal piracy
  more rewarding and make pirating pirates less rewarding, while also
  adding variety to piracy (some factions carry a lot of credits, some
  carry very little).
* Pirates no longer have a one-way hidden jump to Collective space
  (which served no purpose and was essentially just a death trap).
* Adjusted speed and size range of background nebulae and stars so they
  look nicer.
* Ship Stealing targets now lose most of their ammo in addition to armor
  and battery, and their armor regeneration is now disabled.
* Added a limit to mercenary group size in systems with low paying
  faction presence.
* Normalized some wonky presence costs, which in particular prevents
  Za'lek drones from outnumbering pirates in systems where they're
  supposed to have lower presence than them.
* Added a new set of stations and hidden jumps to help pirates get to
  Sirius space.
* The Brushed and Slim GUIs now hide the radar while the overlay is
  open.
* Maps now each have their own individual graphics, rather than all of
  them sharing the same graphic. The graphics are screenshots showing a
  Discovery mode map window showing just the information provided by the
  respective map.
* Improved the way music tracks are chosen, preventing needless music
  changes and allowing nebula ambient music in factional areas.
* Fixed some pirates on pirate worlds being described as civilians.
* Afterburners no longer cause the screen to wobble.
* Removed the "Time Constant" tutorial, both to avoid making the system
  look more significant than it is and because the information about how
  to equip ships is redundant now that this is effectively
  self-documented (with all ships coming with reasonable weapons).
* Removed the ability to delete ship logs, which was a feature that
  didn't have any real utility for the player and risked deleting
  important information.
* Simplified the ship log display, removing the "log type" selector and
  adding a display of the currently selected log.

## 0.1.3

* Fixed Drunkard mission not being able to start up.
* Fixed Nebula Probe mission failing to advance to the next step after
  launching the probe (making it impossible to complete).
* Fixed extra copy of the Pinnacle during the Baron campaign missions.
* Fixed Pinnacle stalling after completing Baron missions (caused by a
  long-standing API bug).
* Fixed Anxious Merchant mission being unable to be accepted.

## 0.1.2

* Properly fixed a segfault in 0.1.0. Version 0.1.1 attempted a fix, but
  the fix turned out to be faulty and led to breakage.

## 0.1.1

Faulty release.

## 0.1.0

Initial release.
