# Real Time Biome Generation
High Performance Real Time Biome Generation/Alteration using Instanced Meshes.

# Modular Inventory with Moddable Items
A mulitplayer server authoritative inventory system with moddable items that contain 'charactertistics' making it easy to create items and allow players to modify them during crafting or afterwards. Including assembler component that handles time based crafting and queuing that handles all the removal and adding of item or resorting. Handles all the transfering of items between actors, ownershop and 'equipping' with generic 'slots' that can be defined to items. Written so it can be used not only for inventories but for containers in the world or other 'modifiable actors' such as vehicles.

# Multiplayer Vehicle With Passager Seats
Vehicles that allow for multiple players to ride, one as the pilot and four others as passangers.
Uses the Gameplay Ability System to give abilities to the player that they are only allowed to use while mounted to there seat or piloting.

# Multiplayer Carriable Packages
Packages that the player can pick up and carry on there back, packages grant an ability via the Gameplay Abilit System. Packages can be dropped on the ground or 'socketed' into other actors that extend the CarriableSocket class.
Battery Socket and Package provided as an example, the battery can be charged at a socket, then used in vehicles or structures to power them.

# Basic Aggro And Threat Generation
System to handle threat generated by players, very basic but handles most case. Simple weighted table that is modified using Gameplay Ability Systems Attributes to apply 'Aggression' and 'Pacification' to alter the threat values.

# Meatball Character System
Much like the old school PS1 games before 'skinning' was possible, this system uses 'meatballs' of character parts to create a full complete character. This system handles all the binding of the characters sections to it rig and handles any replacements based on requests such as equipping armor.
