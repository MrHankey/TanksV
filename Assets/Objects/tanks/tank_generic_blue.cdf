<CharacterDefinition>
 <Model File="objects/tanks/tank_chassis.chr" Material="objects/tanks/tank_turrets_blue"/>
 <AttachmentList>
  <Attachment AName="track_left" Type="CA_BONE" Rotation="1,0,0,0" Position="0,0,0" BoneName="tank_root" Binding="objects/tanks/track_left.cgf" Flags="0"/>
  <Attachment AName="track_right" Type="CA_BONE" Rotation="1,0,0,0" Position="0,0,0" BoneName="tank_root" Binding="objects/tanks/track_right.cgf" Flags="0"/>
  <Attachment AName="turret" Type="CA_BONE" Rotation="1,0,0,0" Position="0,0.69252968,2.05108" BoneName="tank_turret_attachment" Binding="objects/tanks/turret_heavy.chr" Flags="4" Material="objects/tanks/tank_turrets_blue"/>
  <Attachment AName="physics_proxy" Type="CA_BONE" Rotation="1,0,0,0" Position="0,0,0" BoneName="tank_root" Binding="objects/tanks/tank_physics_hitbox.cgf" Flags="4" Material="objects/tanks/tank_turrets"/>
 </AttachmentList>
 <ShapeDeformation COL0="0" COL1="0" COL2="0" COL3="0" COL4="0" COL5="0" COL6="0" COL7="0"/>
</CharacterDefinition>
