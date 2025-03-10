--[[--
Pilot name generating functions

@usage local pilotname = require "pilotname"

@module pilotname
--]]

local fmt = require "fmt"

local pilotname = {}


--[[--
Generate a generic pilot name.

@usage p:rename(pilotname.generic())
   @treturn string Randomly generated generic pilot name.
--]]
function pilotname.generic()
   -- For translation, please keep in mind that these are names. It is
   -- better to translate in an inaccurate way than to translate in a
   -- way that works less well as a name. {number} is replaced by a
   -- number.
   local names = {
      p_("pilot_name", "Ace {number}"),
      p_("pilot_name", "Action {number}"),
      p_("pilot_name", "Alpha {number}"),
      p_("pilot_name", "Aluminum {number}"),
      p_("pilot_name", "Antimatter {number}"),
      p_("pilot_name", "Argon {number}"),
      p_("pilot_name", "Arrow {number}"),
      p_("pilot_name", "Aurochs {number}"),
      p_("pilot_name", "Axis {number}"),
      p_("pilot_name", "Bear {number}"),
      p_("pilot_name", "Beetle {number}"),
      p_("pilot_name", "Beta {number}"),
      p_("pilot_name", "Black Hole {number}"),
      p_("pilot_name", "Blade {number}"),
      p_("pilot_name", "Blizzard {number}"),
      p_("pilot_name", "Blood {number}"),
      p_("pilot_name", "Blue {number}"),
      p_("pilot_name", "Boä {number}"),
      p_("pilot_name", "Boulder {number}"),
      p_("pilot_name", "Bravo {number}"),
      p_("pilot_name", "Bronze {number}"),
      p_("pilot_name", "Bullet {number}"),
      p_("pilot_name", "Cannon {number}"),
      p_("pilot_name", "Carbon {number}"),
      p_("pilot_name", "Chainsaw {number}"),
      p_("pilot_name", "Chariot {number}"),
      p_("pilot_name", "Chi {number}"),
      p_("pilot_name", "Claw {number}"),
      p_("pilot_name", "Click {number}"),
      p_("pilot_name", "Cluster {number}"),
      p_("pilot_name", "Copper {number}"),
      p_("pilot_name", "Cougar {number}"),
      p_("pilot_name", "Crimson {number}"),
      p_("pilot_name", "Crisis {number}"),
      p_("pilot_name", "Crocodile {number}"),
      p_("pilot_name", "Crow {number}"),
      p_("pilot_name", "Cyborg {number}"),
      p_("pilot_name", "Cyclone {number}"),
      p_("pilot_name", "Dagger {number}"),
      p_("pilot_name", "Darkness {number}"),
      p_("pilot_name", "Dart {number}"),
      p_("pilot_name", "Death {number}"),
      p_("pilot_name", "Deity {number}"),
      p_("pilot_name", "Delta {number}"),
      p_("pilot_name", "Demon {number}"),
      p_("pilot_name", "Diamond {number}"),
      p_("pilot_name", "Dire Wolf {number}"),
      p_("pilot_name", "Dragon {number}"),
      p_("pilot_name", "Dread {number}"),
      p_("pilot_name", "Dynamite {number}"),
      p_("pilot_name", "Eagle {number}"),
      p_("pilot_name", "Earthquake {number}"),
      p_("pilot_name", "Echo {number}"),
      p_("pilot_name", "Eclipse {number}"),
      p_("pilot_name", "Electron {number}"),
      p_("pilot_name", "Emerald {number}"),
      p_("pilot_name", "Epsilon {number}"),
      p_("pilot_name", "Eta {number}"),
      p_("pilot_name", "Evader {number}"),
      p_("pilot_name", "Evolution {number}"),
      p_("pilot_name", "Fairy {number}"),
      p_("pilot_name", "Falcon {number}"),
      p_("pilot_name", "Fang {number}"),
      p_("pilot_name", "Fire {number}"),
      p_("pilot_name", "Firefly {number}"),
      p_("pilot_name", "Fox {number}"),
      p_("pilot_name", "Foxtrot {number}"),
      p_("pilot_name", "Gamma {number}"),
      p_("pilot_name", "Glory {number}"),
      p_("pilot_name", "Gold {number}"),
      p_("pilot_name", "Golf {number}"),
      p_("pilot_name", "Green {number}"),
      p_("pilot_name", "Hammer {number}"),
      p_("pilot_name", "Hawk {number}"),
      p_("pilot_name", "Helium {number}"),
      p_("pilot_name", "Hellstorm {number}"),
      p_("pilot_name", "Hero {number}"),
      p_("pilot_name", "Hornet {number}"),
      p_("pilot_name", "Hotel {number}"),
      p_("pilot_name", "Hunter {number}"),
      p_("pilot_name", "Hurricane {number}"),
      p_("pilot_name", "Hydrogen {number}"),
      p_("pilot_name", "Ice {number}"),
      p_("pilot_name", "Ion {number}"),
      p_("pilot_name", "Iota {number}"),
      p_("pilot_name", "Iron {number}"),
      p_("pilot_name", "Kaisar {number}"),
      p_("pilot_name", "Kappa {number}"),
      p_("pilot_name", "Katana {number}"),
      p_("pilot_name", "Ki {number}"),
      p_("pilot_name", "Kilo {number}"),
      p_("pilot_name", "Krypton {number}"),
      p_("pilot_name", "Lambda {number}"),
      p_("pilot_name", "Lantern {number}"),
      p_("pilot_name", "Lawyer {number}"),
      p_("pilot_name", "Legion {number}"),
      p_("pilot_name", "Leopard {number}"),
      p_("pilot_name", "Light {number}"),
      p_("pilot_name", "Lima {number}"),
      p_("pilot_name", "Lion {number}"),
      p_("pilot_name", "Lithium {number}"),
      p_("pilot_name", "Lizard {number}"),
      p_("pilot_name", "Lynx {number}"),
      p_("pilot_name", "Magma {number}"),
      p_("pilot_name", "Mammoth {number}"),
      p_("pilot_name", "Mantis {number}"),
      p_("pilot_name", "Mastodon {number}"),
      p_("pilot_name", "Mech {number}"),
      p_("pilot_name", "Mercury {number}"),
      p_("pilot_name", "Meridian {number}"),
      p_("pilot_name", "Metamorphosis {number}"),
      p_("pilot_name", "Monarch {number}"),
      p_("pilot_name", "Mosquito {number}"),
      p_("pilot_name", "Mouse {number}"),
      p_("pilot_name", "Myu {number}"),
      p_("pilot_name", "Neon {number}"),
      p_("pilot_name", "Neutron {number}"),
      p_("pilot_name", "Ninja {number}"),
      p_("pilot_name", "Nitrogen {number}"),
      p_("pilot_name", "Nova {number}"),
      p_("pilot_name", "Nu {number}"),
      p_("pilot_name", "Number {number}"),
      p_("pilot_name", "Omega {number}"),
      p_("pilot_name", "Omicron {number}"),
      p_("pilot_name", "Orbit {number}"),
      p_("pilot_name", "Orca {number}"),
      p_("pilot_name", "Order {number}"),
      p_("pilot_name", "Panther {number}"),
      p_("pilot_name", "Paradigm {number}"),
      p_("pilot_name", "Pegasus {number}"),
      p_("pilot_name", "Peril {number}"),
      p_("pilot_name", "Phi {number}"),
      p_("pilot_name", "Phoenix {number}"),
      p_("pilot_name", "Photon {number}"),
      p_("pilot_name", "Pi {number}"),
      p_("pilot_name", "Pixie {number}"),
      p_("pilot_name", "Plasma {number}"),
      p_("pilot_name", "Plutonium {number}"),
      p_("pilot_name", "Positron {number}"),
      p_("pilot_name", "Pride {number}"),
      p_("pilot_name", "Proton {number}"),
      p_("pilot_name", "Psi {number}"),
      p_("pilot_name", "Python {number}"),
      p_("pilot_name", "Radium {number}"),
      p_("pilot_name", "Radon {number}"),
      p_("pilot_name", "Rage {number}"),
      p_("pilot_name", "Raptor {number}"),
      p_("pilot_name", "Raven {number}"),
      p_("pilot_name", "Rebirth {number}"),
      p_("pilot_name", "Red {number}"),
      p_("pilot_name", "Rex {number}"),
      p_("pilot_name", "Rho {number}"),
      p_("pilot_name", "Rift {number}"),
      p_("pilot_name", "Ring {number}"),
      p_("pilot_name", "Robin {number}"),
      p_("pilot_name", "Rocket {number}"),
      p_("pilot_name", "Rogue {number}"),
      p_("pilot_name", "Saber {number}"),
      p_("pilot_name", "Saberwing {number}"),
      p_("pilot_name", "Scarecrow {number}"),
      p_("pilot_name", "Scimitar {number}"),
      p_("pilot_name", "Scorpion {number}"),
      p_("pilot_name", "Scythe {number}"),
      p_("pilot_name", "Serpent {number}"),
      p_("pilot_name", "Shadow {number}"),
      p_("pilot_name", "Shark {number}"),
      p_("pilot_name", "Sheath {number}"),
      p_("pilot_name", "Shepherd {number}"),
      p_("pilot_name", "Shield {number}"),
      p_("pilot_Name", "Shinobi {number}"),
      p_("pilot_name", "Shuriken {number}"),
      p_("pilot_name", "Sierra {number}"),
      p_("pilot_name", "Sigma {number}"),
      p_("pilot_name", "Silver {number}"),
      p_("pilot_name", "Siren {number}"),
      p_("pilot_name", "Sky {number}"),
      p_("pilot_name", "Snake {number}"),
      p_("pilot_name", "Sonic {number}"),
      p_("pilot_name", "Spade {number}"),
      p_("pilot_name", "Spear {number}"),
      p_("pilot_name", "Spider {number}"),
      p_("pilot_name", "Spirit {number}"),
      p_("pilot_name", "Starfighter {number}"),
      p_("pilot_name", "Star Killer {number}"),
      p_("pilot_name", "Storm {number}"),
      p_("pilot_name", "Sword {number}"),
      p_("pilot_name", "System {number}"),
      p_("pilot_name", "Talon {number}"),
      p_("pilot_name", "Tango {number}"),
      p_("pilot_name", "Tau {number}"),
      p_("pilot_name", "Theta {number}"),
      p_("pilot_name", "Thorium {number}"),
      p_("pilot_name", "Thunder {number}"),
      p_("pilot_name", "Tiger {number}"),
      p_("pilot_name", "Titan {number}"),
      p_("pilot_name", "Torch {number}"),
      p_("pilot_name", "Tornado {number}"),
      p_("pilot_name", "Torpedo {number}"),
      p_("pilot_name", "Tortoise {number}"),
      p_("pilot_name", "Tree {number}"),
      p_("pilot_name", "Tsunami {number}"),
      p_("pilot_name", "Turtle {number}"),
      p_("pilot_name", "Twilight {number}"),
      p_("pilot_name", "Typhoon {number}"),
      p_("pilot_name", "Unicorn {number}"),
      p_("pilot_name", "Uniform {number}"),
      p_("pilot_name", "Upsilon {number}"),
      p_("pilot_name", "Uranium {number}"),
      p_("pilot_name", "Vector {number}"),
      p_("pilot_name", "Velocity {number}"),
      p_("pilot_name", "Vengeance {number}"),
      p_("pilot_name", "Virus {number}"),
      p_("pilot_name", "Vision {number}"),
      p_("pilot_name", "Void {number}"),
      p_("pilot_name", "Warlock {number}"),
      p_("pilot_name", "Wasp {number}"),
      p_("pilot_name", "Watcher {number}"),
      p_("pilot_name", "Whiskey {number}"),
      p_("pilot_name", "Wind {number}"),
      p_("pilot_name", "Wing {number}"),
      p_("pilot_name", "Wolf {number}"),
      p_("pilot_name", "Wraith {number}"),
      p_("pilot_name", "Xenon {number}"),
      p_("pilot_name", "Xi {number}"),
      p_("pilot_name", "X-ray {number}"),
      p_("pilot_name", "Yankee {number}"),
      p_("pilot_name", "Zebra {number}"),
      p_("pilot_name", "Zeppelin {number}"),
      p_("pilot_name", "Zeta {number}"),
      p_("pilot_name", "Zero {number}"),
      p_("pilot_name", "Zombie {number}"),
   }
   local name = names[rnd.rnd(1, #names)]
   return fmt.f(name, {number=rnd.rnd(1, 99)})
end


--[[--
Generate a pirate name.

@usage p:rename(pilotname.pirate())
   @treturn string Randomly generated pirate name.
--]]
function pilotname.pirate()
   -- For translation, please keep in mind that these are names. It is
   -- better to translate in an inaccurate way than to translate in a
   -- way that works less well as a name. Components are pieced together
   -- based on a randomly chosen format.
   local articles = {
      p_("pirate_name_article_neuter", "The"),
      p_("pirate_name_article_masculine", "The"),
      p_("pirate_name_article_feminine", "The"),
   }
   local descriptors = {
      p_("pirate_name_descriptor", "Aerobic"),
      p_("pirate_name_descriptor", "Ancient"),
      p_("pirate_name_descriptor", "Armed"),
      p_("pirate_name_descriptor", "Baggy"),
      p_("pirate_name_descriptor", "Bald"),
      p_("pirate_name_descriptor", "Beautiful"),
      p_("pirate_name_descriptor", "Benevolent"),
      p_("pirate_name_descriptor", "Big"),
      p_("pirate_name_descriptor", "Big Bad"),
      p_("pirate_name_descriptor", "Bloody"),
      p_("pirate_name_descriptor", "Bright"),
      p_("pirate_name_descriptor", "Brooding"),
      p_("pirate_name_descriptor", "Caped"),
      p_("pirate_name_descriptor", "Citrus"),
      p_("pirate_name_descriptor", "Clustered"),
      p_("pirate_name_descriptor", "Cocky"),
      p_("pirate_name_descriptor", "Creamy"),
      p_("pirate_name_descriptor", "Crisis"),
      p_("pirate_name_descriptor", "Crusty"),
      p_("pirate_name_descriptor", "Dark"),
      p_("pirate_name_descriptor", "Deadly"),
      p_("pirate_name_descriptor", "Death"),
      p_("pirate_name_descriptor", "Deathly"),
      p_("pirate_name_descriptor", "Defiant"),
      p_("pirate_name_descriptor", "Delicious"),
      p_("pirate_name_descriptor", "Despicable"),
      p_("pirate_name_descriptor", "Destructive"),
      p_("pirate_name_descriptor", "Diligent"),
      p_("pirate_name_descriptor", "Drunk"),
      p_("pirate_name_descriptor", "Egotistical"),
      p_("pirate_name_descriptor", "Electromagnetic"),
      p_("pirate_name_descriptor", "Erroneous"),
      p_("pirate_name_descriptor", "Escaped"),
      p_("pirate_name_descriptor", "Eternal"),
      p_("pirate_name_descriptor", "Evil"),
      p_("pirate_name_descriptor", "Explosive"),
      p_("pirate_name_descriptor", "Fallen"),
      p_("pirate_name_descriptor", "Fearless"),
      p_("pirate_name_descriptor", "Fearsome"),
      p_("pirate_name_descriptor", "Filthy"),
      p_("pirate_name_descriptor", "Flightless"),
      p_("pirate_name_descriptor", "Flying"),
      p_("pirate_name_descriptor", "Foreboding"),
      p_("pirate_name_descriptor", "Full-Motion"),
      p_("pirate_name_descriptor", "General"),
      p_("pirate_name_descriptor", "Gigantic"),
      p_("pirate_name_descriptor", "Glittery"),
      p_("pirate_name_descriptor", "Glorious"),
      p_("pirate_name_descriptor", "Great"),
      p_("pirate_name_descriptor", "Grumpy"),
      p_("pirate_name_descriptor", "Hairy"),
      p_("pirate_name_descriptor", "Hammy"),
      p_("pirate_name_descriptor", "Handsome"),
      p_("pirate_name_descriptor", "Happy"),
      p_("pirate_name_descriptor", "Hilarious"),
      p_("pirate_name_descriptor", "Horrible"),
      p_("pirate_name_descriptor", "Imperial"),
      p_("pirate_name_descriptor", "Impressive"),
      p_("pirate_name_descriptor", "Insatiable"),
      p_("pirate_name_descriptor", "Ionic"),
      p_("pirate_name_descriptor", "Iron"),
      p_("pirate_name_descriptor", "Justice"),
      p_("pirate_name_descriptor", "Last"),
      p_("pirate_name_descriptor", "Learnèd"),
      p_("pirate_name_descriptor", "Lesser"),
      p_("pirate_name_descriptor", "Lightspeed"),
      p_("pirate_name_descriptor", "Lone"),
      p_("pirate_name_descriptor", "Loose"),
      p_("pirate_name_descriptor", "Loud"),
      p_("pirate_name_descriptor", "Lovely"),
      p_("pirate_name_descriptor", "Lustful"),
      p_("pirate_name_descriptor", "Malodorous"),
      p_("pirate_name_descriptor", "Mellow"),
      p_("pirate_name_descriptor", "Messy"),
      p_("pirate_name_descriptor", "Mighty"),
      p_("pirate_name_descriptor", "Morbid"),
      p_("pirate_name_descriptor", "Murderous"),
      p_("pirate_name_descriptor", "Naïve"),
      p_("pirate_name_descriptor", "New"),
      p_("pirate_name_descriptor", "Night's"),
      p_("pirate_name_descriptor", "Nimble"),
      p_("pirate_name_descriptor", "No Good"),
      p_("pirate_name_descriptor", "Numb"),
      p_("pirate_name_descriptor", "Old"),
      p_("pirate_name_descriptor", "Pale"),
      p_("pirate_name_descriptor", "Perilous"),
      p_("pirate_name_descriptor", "Pirate's"),
      p_("pirate_name_descriptor", "Pocket"),
      p_("pirate_name_descriptor", "Princeless"),
      p_("pirate_name_descriptor", "Psychic"),
      p_("pirate_name_descriptor", "Quick"),
      p_("pirate_name_descriptor", "Radical"),
      p_("pirate_name_descriptor", "Raging"),
      p_("pirate_name_descriptor", "Reclusive"),
      p_("pirate_name_descriptor", "Relentless"),
      p_("pirate_name_descriptor", "Rough"),
      p_("pirate_name_descriptor", "Ruthless"),
      p_("pirate_name_descriptor", "Saccharin"),
      p_("pirate_name_descriptor", "Salty"),
      p_("pirate_name_descriptor", "Satanic"),
      p_("pirate_name_descriptor", "Secluded"),
      p_("pirate_name_descriptor", "Serial"),
      p_("pirate_name_descriptor", "Sharing"),
      p_("pirate_name_descriptor", "Silly"),
      p_("pirate_name_descriptor", "Single"),
      p_("pirate_name_descriptor", "Sleepy"),
      p_("pirate_name_descriptor", "Slimy"),
      p_("pirate_name_descriptor", "Smelly"),
      p_("pirate_name_descriptor", "Solar"),
      p_("pirate_name_descriptor", "Sonic"),
      p_("pirate_name_descriptor", "Space"),
      p_("pirate_name_descriptor", "Spicy"),
      p_("pirate_name_descriptor", "Spirit"),
      p_("pirate_name_descriptor", "Stained"),
      p_("pirate_name_descriptor", "Static"),
      p_("pirate_name_descriptor", "Steel"),
      p_("pirate_name_descriptor", "Strange"),
      p_("pirate_name_descriptor", "Strawhat"),
      p_("pirate_name_descriptor", "Super"),
      p_("pirate_name_descriptor", "Supreme"),
      p_("pirate_name_descriptor", "Sweet"),
      p_("pirate_name_descriptor", "Tall"),
      p_("pirate_name_descriptor", "Terrible"),
      p_("pirate_name_descriptor", "Tired"),
      p_("pirate_name_descriptor", "Titanic"),
      p_("pirate_name_descriptor", "Toothless"),
      p_("pirate_name_descriptor", "Tropical"),
      p_("pirate_name_descriptor", "Typical"),
      p_("pirate_name_descriptor", "Ultimate"),
      p_("pirate_name_descriptor", "Uncombed"),
      p_("pirate_name_descriptor", "Undead"),
      p_("pirate_name_descriptor", "Unhealthy"),
      p_("pirate_name_descriptor", "Unreal"),
      p_("pirate_name_descriptor", "Unsightly"),
      p_("pirate_name_descriptor", "Vengeful"),
      p_("pirate_name_descriptor", "Very Bad"),
      p_("pirate_name_descriptor", "Vindictive"),
      p_("pirate_name_descriptor", "Violent"),
      p_("pirate_name_descriptor", "Weeping"),
      p_("pirate_name_descriptor", "Wild"),
      p_("pirate_name_descriptor", "Wiley"),
      p_("pirate_name_descriptor", "Winged"),
      p_("pirate_name_descriptor", "Wretched"),
      p_("pirate_name_descriptor", "Yummy"),
      p_("pirate_name_descriptor", "Zany"),
   }
   local colors = {
      p_("pirate_name_color", "Amber"),
      p_("pirate_name_color", "Aquamarine"),
      p_("pirate_name_color", "Azure"),
      p_("pirate_name_color", "Beige"),
      p_("pirate_name_color", "Blue"),
      p_("pirate_name_color", "Chartreuse"),
      p_("pirate_name_color", "Crimson"),
      p_("pirate_name_color", "Cyan"),
      p_("pirate_name_color", "Emerald"),
      p_("pirate_name_color", "Fuchsia"),
      p_("pirate_name_color", "Golden"),
      p_("pirate_name_color", "Gray"),
      p_("pirate_name_color", "Green"),
      p_("pirate_name_color", "Indigo"),
      p_("pirate_name_color", "Ivory"),
      p_("pirate_name_color", "Khaki"),
      p_("pirate_name_color", "Lavender"),
      p_("pirate_name_color", "Magenta"),
      p_("pirate_name_color", "Maroon"),
      p_("pirate_name_color", "Mauve"),
      p_("pirate_name_color", "Olive"),
      p_("pirate_name_color", "Pink"),
      p_("pirate_name_color", "Purple"),
      p_("pirate_name_color", "Red"),
      p_("pirate_name_color", "Silver"),
      p_("pirate_name_color", "Teal"),
      p_("pirate_name_color", "Turquoise"),
      p_("pirate_name_color", "Violet"),
      p_("pirate_name_color", "Yellow"),
   }
   local actors = {
      p_("pirate_name_actor", "Alphabet Soup"),
      p_("pirate_name_actor", "Angel"),
      p_("pirate_name_actor", "Angle Grinder"),
      p_("pirate_name_actor", "Anvil"),
      p_("pirate_name_actor", "Arrow"),
      p_("pirate_name_actor", "Ass"),
      p_("pirate_name_actor", "Aunt"),
      p_("pirate_name_actor", "Avenger"),
      p_("pirate_name_actor", "Axis"),
      p_("pirate_name_actor", "Ball"),
      p_("pirate_name_actor", "Band Saw"),
      p_("pirate_name_actor", "Bat"),
      p_("pirate_name_actor", "Beam"),
      p_("pirate_name_actor", "Beard"),
      p_("pirate_name_actor", "Belt Sander"),
      p_("pirate_name_actor", "Bench Grinder"),
      p_("pirate_name_actor", "Black Hole"),
      p_("pirate_name_actor", "Blizzard"),
      p_("pirate_name_actor", "Blood"),
      p_("pirate_name_actor", "Blunder"),
      p_("pirate_name_actor", "Boom"),
      p_("pirate_name_actor", "Boot"),
      p_("pirate_name_actor", "Bolt Cutter"),
      p_("pirate_name_actor", "Brain"),
      p_("pirate_name_actor", "Breeze"),
      p_("pirate_name_actor", "Bride"),
      p_("pirate_name_actor", "Brigand"),
      p_("pirate_name_actor", "Bulk"),
      p_("pirate_name_actor", "Burglar"),
      p_("pirate_name_actor", "Cane"),
      p_("pirate_name_actor", "Cannon"),
      p_("pirate_name_actor", "Chainsaw"),
      p_("pirate_name_actor", "Cheese"),
      p_("pirate_name_actor", "Cheese Grater"),
      p_("pirate_name_actor", "Chi"),
      p_("pirate_name_actor", "Chicken"),
      p_("pirate_name_actor", "Chin"),
      p_("pirate_name_actor", "Circle"),
      p_("pirate_name_actor", "Claw"),
      p_("pirate_name_actor", "Claw Hammer"),
      p_("pirate_name_actor", "Club"),
      p_("pirate_name_actor", "Coconut"),
      p_("pirate_name_actor", "Coot"),
      p_("pirate_name_actor", "Corsair"),
      p_("pirate_name_actor", "Cougar"),
      p_("pirate_name_actor", "Coyote"),
      p_("pirate_name_actor", "Crisis"),
      p_("pirate_name_actor", "Crow"),
      p_("pirate_name_actor", "Crowbar"),
      p_("pirate_name_actor", "Crusader"),
      p_("pirate_name_actor", "Curse"),
      p_("pirate_name_actor", "Cyborg"),
      p_("pirate_name_actor", "Darkness"),
      p_("pirate_name_actor", "Death"),
      p_("pirate_name_actor", "Deity"),
      p_("pirate_name_actor", "Demon"),
      p_("pirate_name_actor", "Destruction"),
      p_("pirate_name_actor", "Devil"),
      p_("pirate_name_actor", "Dictator"),
      p_("pirate_name_actor", "Disaster"),
      p_("pirate_name_actor", "Discord"),
      p_("pirate_name_actor", "Donkey"),
      p_("pirate_name_actor", "Doom"),
      p_("pirate_name_actor", "Dragon"),
      p_("pirate_name_actor", "Dread"),
      p_("pirate_name_actor", "Drifter"),
      p_("pirate_name_actor", "Drill Press"),
      p_("pirate_name_actor", "Duckling"),
      p_("pirate_name_actor", "Eagle"),
      p_("pirate_name_actor", "Eggplant"),
      p_("pirate_name_actor", "Ego"),
      p_("pirate_name_actor", "Electricity"),
      p_("pirate_name_actor", "Emperor"),
      p_("pirate_name_actor", "Engine"),
      p_("pirate_name_actor", "Fang"),
      p_("pirate_name_actor", "Flare"),
      p_("pirate_name_actor", "Flash"),
      p_("pirate_name_actor", "Fox"),
      p_("pirate_name_actor", "Friend"),
      p_("pirate_name_actor", "Fruit"),
      p_("pirate_name_actor", "Fugitive"),
      p_("pirate_name_actor", "Game"),
      p_("pirate_name_actor", "Gamma Ray"),
      p_("pirate_name_actor", "Giant"),
      p_("pirate_name_actor", "Gift"),
      p_("pirate_name_actor", "Goose"),
      p_("pirate_name_actor", "Gorilla"),
      p_("pirate_name_actor", "Gun"),
      p_("pirate_name_actor", "Halapenyo"),
      p_("pirate_name_actor", "Hamburger"),
      p_("pirate_name_actor", "Hammer"),
      p_("pirate_name_actor", "Headache"),
      p_("pirate_name_actor", "Hedgehog"),
      p_("pirate_name_actor", "Hero"),
      p_("pirate_name_actor", "Hex"),
      p_("pirate_name_actor", "Horror"),
      p_("pirate_name_actor", "Hunter"),
      p_("pirate_name_actor", "Hurricane"),
      p_("pirate_name_actor", "Husband"),
      p_("pirate_name_actor", "Id"),
      p_("pirate_name_actor", "Impact Wrench"),
      p_("pirate_name_actor", "Ionizer"),
      p_("pirate_name_actor", "Jigsaw"),
      p_("pirate_name_actor", "Jinx"),
      p_("pirate_name_actor", "Kamikazé"),
      p_("pirate_name_actor", "Kappa"),
      p_("pirate_name_actor", "Karaoké"),
      p_("pirate_name_actor", "Katana"),
      p_("pirate_name_actor", "Keel"),
      p_("pirate_name_actor", "Ketchup"),
      p_("pirate_name_actor", "Ki"),
      p_("pirate_name_actor", "Killer"),
      p_("pirate_name_actor", "King"),
      p_("pirate_name_actor", "Kitchen Knife"),
      p_("pirate_name_actor", "Kitten"),
      p_("pirate_name_actor", "Knave"),
      p_("pirate_name_actor", "Knife"),
      p_("pirate_name_actor", "Knight"),
      p_("pirate_name_actor", "Lance"),
      p_("pirate_name_actor", "Lantern"),
      p_("pirate_name_actor", "Lawyer"),
      p_("pirate_name_actor", "League"),
      p_("pirate_name_actor", "Lemon"),
      p_("pirate_name_actor", "Lime"),
      p_("pirate_name_actor", "Lust"),
      p_("pirate_name_actor", "Magician"),
      p_("pirate_name_actor", "Magma"),
      p_("pirate_name_actor", "Maize"),
      p_("pirate_name_actor", "Mango"),
      p_("pirate_name_actor", "Mastodon"),
      p_("pirate_name_actor", "Mech"),
      p_("pirate_name_actor", "Melon"),
      p_("pirate_name_actor", "Metamorphosis"),
      p_("pirate_name_actor", "Mind"),
      p_("pirate_name_actor", "Model"),
      p_("pirate_name_actor", "Monster"),
      p_("pirate_name_actor", "Mosquito"),
      p_("pirate_name_actor", "Mustache"),
      p_("pirate_name_actor", "Mushroom"),
      p_("pirate_name_actor", "Mustard"),
      p_("pirate_name_actor", "Necromancer"),
      p_("pirate_name_actor", "Night"),
      p_("pirate_name_actor", "Ninja"),
      p_("pirate_name_actor", "Nova"),
      p_("pirate_name_actor", "Octopus"),
      p_("pirate_name_actor", "Ogre"),
      p_("pirate_name_actor", "Oni"),
      p_("pirate_name_actor", "Onion"),
      p_("pirate_name_actor", "Osiris"),
      p_("pirate_name_actor", "Outlaw"),
      p_("pirate_name_actor", "Oyster"),
      p_("pirate_name_actor", "Panther"),
      p_("pirate_name_actor", "Paste"),
      p_("pirate_name_actor", "Pea"),
      p_("pirate_name_actor", "Peapod"),
      p_("pirate_name_actor", "Peril"),
      p_("pirate_name_actor", "Pickaxe"),
      p_("pirate_name_actor", "Pipe Wrench"),
      p_("pirate_name_actor", "Pitchfork"),
      p_("pirate_name_actor", "Politician"),
      p_("pirate_name_actor", "Potato"),
      p_("pirate_name_actor", "Potter"),
      p_("pirate_name_actor", "Pride"),
      p_("pirate_name_actor", "Princess"),
      p_("pirate_name_actor", "Pulse"),
      p_("pirate_name_actor", "Puppy"),
      p_("pirate_name_actor", "Python"),
      p_("pirate_name_actor", "Quack"),
      p_("pirate_name_actor", "Queen"),
      p_("pirate_name_actor", "Radio"),
      p_("pirate_name_actor", "Ramen"),
      p_("pirate_name_actor", "Rat"),
      p_("pirate_name_actor", "Ravager"),
      p_("pirate_name_actor", "Raven"),
      p_("pirate_name_actor", "Reaver"),
      p_("pirate_name_actor", "Recluse"),
      p_("pirate_name_actor", "Rice"),
      p_("pirate_name_actor", "Ring"),
      p_("pirate_name_actor", "River"),
      p_("pirate_name_actor", "Rubber"),
      p_("pirate_name_actor", "Rubber Mallet"),
      p_("pirate_name_actor", "Rush"),
      p_("pirate_name_actor", "Salad"),
      p_("pirate_name_actor", "Samurai"),
      p_("pirate_name_actor", "Saucer"),
      p_("pirate_name_actor", "Scythe"),
      p_("pirate_name_actor", "Sea"),
      p_("pirate_name_actor", "Seaweed"),
      p_("pirate_name_actor", "Sentinel"),
      p_("pirate_name_actor", "Serpent"),
      p_("pirate_name_actor", "Shepherd"),
      p_("pirate_name_actor", "Shinobi"),
      p_("pirate_name_actor", "Shock"),
      p_("pirate_name_actor", "Shovel"),
      p_("pirate_name_actor", "Siren"),
      p_("pirate_name_actor", "Slayer"),
      p_("pirate_name_actor", "Space Dog"),
      p_("pirate_name_actor", "Spade"),
      p_("pirate_name_actor", "Spaghetti"),
      p_("pirate_name_actor", "Spaghetti Monster"),
      p_("pirate_name_actor", "Spearmint"),
      p_("pirate_name_actor", "Spider"),
      p_("pirate_name_actor", "Spirit"),
      p_("pirate_name_actor", "Squeegee"),
      p_("pirate_name_actor", "Squid"),
      p_("pirate_name_actor", "Staple Gun"),
      p_("pirate_name_actor", "Stir-fry"),
      p_("pirate_name_actor", "Storm"),
      p_("pirate_name_actor", "Straw"),
      p_("pirate_name_actor", "Supernova"),
      p_("pirate_name_actor", "Surstrumming"),
      p_("pirate_name_actor", "Table Saw"),
      p_("pirate_name_actor", "Taco"),
      p_("pirate_name_actor", "Terror"),
      p_("pirate_name_actor", "Thunder"),
      p_("pirate_name_actor", "Tooth"),
      p_("pirate_name_actor", "Treasure Hunter"),
      p_("pirate_name_actor", "Tree"),
      p_("pirate_name_actor", "Tumbler"),
      p_("pirate_name_actor", "Turret Lathe"),
      p_("pirate_name_actor", "Twilight"),
      p_("pirate_name_actor", "Tyrant"),
      p_("pirate_name_actor", "Underdog"),
      p_("pirate_name_actor", "Urchin"),
      p_("pirate_name_actor", "Velocity"),
      p_("pirate_name_actor", "Vengeance"),
      p_("pirate_name_actor", "Void"),
      p_("pirate_name_actor", "Vomit"),
      p_("pirate_name_actor", "Watcher"),
      p_("pirate_name_actor", "Wedge"),
      p_("pirate_name_actor", "Widget"),
      p_("pirate_name_actor", "Widow"),
      p_("pirate_name_actor", "Wight"),
      p_("pirate_name_actor", "Willow"),
      p_("pirate_name_actor", "Wind"),
      p_("pirate_name_actor", "Wizard"),
      p_("pirate_name_actor", "Wolf"),
      p_("pirate_name_actor", "X-Ray"),
      p_("pirate_name_actor", "Xylophone"),
      p_("pirate_name_actor", "Yakuza"),
      p_("pirate_name_actor", "Zebra"),
      p_("pirate_name_actor", "Zombie"),
   }
   local formats = {
      -- These are name formats; please adjust as appropriate for the
      -- target language, including rearranging components as needed.
      -- The {article}, {descriptor}, {color}, and {actor} tags can
      -- appear in any of them and those that exist in the English
      -- version can be erased as well if the combination doesn't make
      -- sense in the target language.
      p_("pirate_name_format", "{article} {actor}"),
      p_("pirate_name_format", "{color} {actor}"),
      p_("pirate_name_format", "{descriptor} {actor}"),
      p_("pirate_name_format", "{article} {descriptor} {actor}"),
      p_("pirate_name_format", "{article} {color} {actor}"),
      p_("pirate_name_format", "{article} {descriptor} {color} {actor}"),
   }
   local article = articles[rnd.rnd(1, #articles)]
   local descriptor = descriptors[rnd.rnd(1, #descriptors)]
   local color = colors[rnd.rnd(1, #colors)]
   local actor = actors[rnd.rnd(1, #actors)]
   local format = formats[rnd.rnd(1, #formats)]

   return fmt.f(format,
         {article=article, descriptor=descriptor, color=color, actor=actor})
end


return pilotname
