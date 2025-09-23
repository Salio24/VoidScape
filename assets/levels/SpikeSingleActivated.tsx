<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.2" name="SpikeSingleActivated" tilewidth="16" tileheight="16" tilecount="1" columns="1">
 <image source="../textures/level/Spikes/SpikeSingleActivated.png" width="16" height="16"/>
 <tile id="0">
  <properties>
   <property name="Damage" type="float" value="60"/>
   <property name="HitImpulseX" type="float" value="0"/>
   <property name="HitImpulseY" type="float" value="15"/>
   <property name="HitPlayerVelocityModifierX" type="float" value="1"/>
   <property name="HitPlayerVelocityModifierY" type="float" value="0"/>
   <property name="HitStunTicksPenalty" type="int" value="0"/>
   <property name="InvisibilityTicksBonus" type="int" value="30"/>
  </properties>
  <objectgroup draworder="index" id="2">
   <object id="1" x="7" y="16">
    <polygon points="0,0 -2,-2 -2,-4 0,-7 1,-7 3,-4 3,-2 1,0"/>
   </object>
  </objectgroup>
 </tile>
</tileset>
