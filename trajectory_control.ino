void control_change_pitch(){
    float range_for_change = pitch_tune - pid_pitch_setpoint;
    if (range_for_change<0){
      pid_pitch_setpoint-=0.1;
    }
    if (range_for_change>0){
      pid_pitch_setpoint+=0.1;
    }
    if (range_for_change==0){
      
      }
}

void control_change_roll(){
    float range_for_change = roll_tune - pid_roll_setpoint;
    if (range_for_change<0){
      pid_roll_setpoint-=0.1;
    }
    if (range_for_change>0){
      pid_roll_setpoint+=0.1;
    }
    if (range_for_change==0){
      
      }
}
