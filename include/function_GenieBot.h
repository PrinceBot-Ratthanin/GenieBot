bool isYawAligned(float yaw, float tolerance = 2.0) {
    int angles[] = {0, 90, 180, 270, 360};
    for (int i = 0; i < 5; i++) {
        float diff = fabs(yaw - angles[i]);
        if (diff <= tolerance || fabs(diff - 360.0) <= tolerance) {
            return true;
        }
    }
    return false;
}
void wait_SW1() {
  int state_waitSW1 = 0;
  display.clearDisplay();
  display.setTextSize(1);
  do{
    display.clearDisplay();
    drawString(0,0,String(String("A1::")));
    drawString(28,0,String(ADC(1)));
    drawString(65,0,String(String("A2::")));
    drawString(93,0,String(ADC(2)));
    drawString(0,10,String(String("A3::")));
    drawString(28,10,String(ADC(3)));
    drawString(65,10,String(String("A4::")));
    drawString(93,10,String(ADC(4)));
    drawString(0,20,String(String("A5::")));
    drawString(28,20,String(ADC(5)));
    drawString(65,20,String(String("A6::")));
    drawString(93,20,String(ADC(6)));
    drawString(0,30,String(String("A7::")));
    drawString(28,30,String(ADC(7)));
    drawString(65,30,String(String("A8::")));
    drawString(93,30,String(ADC(8)));
    drawString(0,40,String(String("A9::")));
    drawString(28,40,String(ADC(9)));
    drawString(65,40,String(String("E:")));
    drawString(93,40,String(get_pulse_Encoder()));
    drawString(0,50,String(String("Gy:")));
    drawString(20,50,String(getContinuousYaw()));
    drawString(65,50,String(String("Dis:")));
    drawString(90,50,String(sensor.getDistance()));

    display.display();
    delay(50);
  } while (digitalRead(15) == 1);
  
}
void moveStraightPID_Encoder_andSensor(int Movedirection, float targetYaw_straight, int speedBase, int target_encoder, float kp_straight, float ki_straight, float kd_straight,int function) {
    long initialEncoderCount = get_pulse_Encoder();
    unsigned long startTime = millis();
    float integral_straight = 0, preverror_straight = 0;
    while (abs(get_pulse_Encoder() - initialEncoderCount) < target_encoder) { 
    	if(function == 0){
    		if(Read_status_sensor(7) and Read_status_sensor(8)){break;}
    	}
    	else if(function == 1){
    		if(Read_status_sensor(7)){break;}
    	}
    	else if(function == 2){
    		if(Read_status_sensor(8)){break;}
    	}
        updateContinuousYaw();  
        float current_Yaw = getContinuousYaw();
        float error_straight = normalizeAngle(targetYaw_straight - current_Yaw);

        static unsigned long previousTime = millis();
		unsigned long now = millis();
		float deltaTime = max((now - previousTime) / 1000.0, 0.001);
		previousTime = now;

        float derivative = (error_straight - preverror_straight) / deltaTime;
        float output = kp_straight * error_straight  + kd_straight * derivative;

        preverror_straight = error_straight;

        int rightSpeed;
        int leftSpeed;

          rightSpeed = constrain(speedBase - output, 0, 100);
          leftSpeed = constrain(speedBase + output, 0, 100);

        if (Movedirection == 0) {  // เดินหน้า
            motor(1, leftSpeed);
            motor(2, rightSpeed);

        } else {  // ถอยหลัง
            motor(1, -rightSpeed);
            motor(2, -leftSpeed);

        }
    }

    ao(); delay(10);
    ao(); 
}
int getClosestAngle45(float currentAngle) {
    // ปรับให้อยู่ในช่วง 0-360
    while (currentAngle < 0) currentAngle += 360;
    while (currentAngle >= 360) currentAngle -= 360;

    int angles[] = {0,45, 90,135, 180,225, 270,315, 360};
    int closest = angles[0];
    float minDiff = fabs(currentAngle - angles[0]);

    for (int i = 1; i < 5; i++) {
        float diff = fabs(currentAngle - angles[i]);
        if (diff < minDiff) {
            minDiff = diff;
            closest = angles[i];
        }
    }

    // แปลง 360 กลับเป็น 0 เพื่อความสอดคล้อง
    if (closest == 360) closest = 0;

    return closest;
}
void moveStraightSnapToNearest_andSensor(int Movedirection, int speedBase, int target_encoder, float kp, float ki, float kd, int function) {
  for (int i = 0; i < 5; i++) updateContinuousYaw();
    // updateContinuousYaw(); // สำคัญมาก ต้องอัปเดตค่า Yaw ก่อนอ่าน
    float currentYaw = getContinuousYaw();   // อ่านมุมจริงจาก Gyro
	//display.clearDisplay();display.setTextSize(2);drawString(0, 0, String(currentYaw));display.display();
    int snappedYaw = getClosestAngle45(currentYaw); // หามุมเป้าหมายใกล้สุด เช่น 0, 90, ...
   //display.setTextSize(2);drawString(0, 20, String(snappedYaw));display.display();
    moveStraightPID_Encoder_andSensor(Movedirection, snappedYaw, speedBase, target_encoder, kp, ki, kd, function);
}
int readline_2(int function_read)   
{                
  int onLine = 0;
  long avg = 0;
  long sum = 0;
  for (uint8_t i = 0; i < PID_NumPin ; i++) 
  {
    long value;
    if(Front_color == 0){
      value = map(ADC(PID_SetupPin[i]), PID_Min[i], PID_Max[i], 100, 0);
    }
    else {
      value = map(ADC(PID_SetupPin[i]), PID_Min[i], PID_Max[i], 0, 100);
    }
    value = constrain(value,0,100);      
    if (value > 100 - 50) { 
      onLine = 1;
    }
    if (value > 5){
      avg += (long)value * (i * 100)+ 50;
      sum += value;
    }
  }
   if (onLine == 0)
  {
    
    if(function_read == 0){
        if (_lastPosition < 300) {
          return 0; // หันซ้าย
        } else  {
          return (600); // หันขวา
        } 
    }
    else if(function_read == 1){
        int leftThreshold = ((PID_NumPin - 1) * 100) / 3;
        int rightThreshold = ((PID_NumPin - 1) * 100) * 2 / 3;
        int center = ((PID_NumPin - 1) * 100) / 2;

        if (_lastPosition < leftThreshold) {
          return 0; // หันซ้าย
        } else if (_lastPosition > rightThreshold) {
          return ((PID_NumPin - 1) * 100); // หันขวา
        } else {
          return center; // วิ่งตรง
        }
    }
    else if(function_read == 2){
        return 0;
    }
    else if(function_read == 3){
        return 600;
    }
    else{
        return 300;
    }
  }

  if (sum == 0) return _lastPosition; // ป้องกันหาร 0
  _lastPosition = avg / sum;
  return _lastPosition;
}
void Run_PID_until_frontSensor(int RUN_PID_speed,float RUN_PID_KP,float RUN_PID_KD,int sumValue_traget,int function_read){
  do
  {
    int present_position = readline_2(function_read);
    int setpoint = ((PID_NumPin - 1) * 100) / 2;
    errors = present_position - setpoint;
	static int previous_error = 0;
    derivative = (errors - previous_error) ;
    output = RUN_PID_KP * errors  + RUN_PID_KD * derivative;
    
    int m1Speed = RUN_PID_speed + output ;
    int m2Speed = RUN_PID_speed - output;
    m1Speed = constrain(m1Speed, -80, 80);
    m2Speed = constrain(m2Speed, -80, 80);
    motor(1,m1Speed);
    motor(2,m2Speed);
    delay(1);
    if((Read_status_sensor(0) and Read_status_sensor(1) and Read_status_sensor(3)) or Read_status_sensor(3) and Read_status_sensor(5)and Read_status_sensor(6) ){
    	break;
    }
    previous_error = errors;
  }while(Read_sumValue_sensor() < sumValue_traget);
}
void Run_PID_until_encoder(int RUN_PID_speed,float RUN_PID_KP,float RUN_PID_KD,int target_encoder,int function_read){
    long initialEncoderCount = get_pulse_Encoder();
  	
  do
  {
    int present_position = readline_2(function_read);
    int setpoint = ((PID_NumPin - 1) * 100) / 2;
    errors = present_position - setpoint;
	static int previous_error = 0;
    derivative = (errors - previous_error) ;
    output = RUN_PID_KP * errors  + RUN_PID_KD * derivative;
    
    int m1Speed = RUN_PID_speed + output ;
    int m2Speed = RUN_PID_speed - output;
    m1Speed = constrain(m1Speed, -80, 80);
    m2Speed = constrain(m2Speed, -80, 80);
    motor(1,m1Speed);
    motor(2,m2Speed);
    delay(2);
    previous_error = errors;

  }while (abs(get_pulse_Encoder() - initialEncoderCount) < target_encoder) ;
}
void Run_PID_until_can(int RUN_PID_speed,float RUN_PID_KP,float RUN_PID_KD,int target_encoder,int function_read){   
  do
  {
    int present_position = readline_2(function_read);
    int setpoint = ((PID_NumPin - 1) * 100) / 2;
    errors = present_position - setpoint;
    static int previous_error = 0;
    derivative = (errors - previous_error) ;
    output = RUN_PID_KP * errors  + RUN_PID_KD * derivative;
    int m1Speed = RUN_PID_speed + output;
    int m2Speed = RUN_PID_speed - output;
    m1Speed = constrain(m1Speed, -80, 80);
    m2Speed = constrain(m2Speed, -80, 80);
    motor(1,m1Speed);
    motor(2,m2Speed);
    delay(1);
    previous_error = errors;
    int Distance = sensor.getDistance();
    if(Distance < target_encoder){
        ao();
        delay(100);
        display.clearDisplay();
        display.setTextSize(2);
        drawString(0, 0, String(Distance));
        display.display();
        break;
    }

  }while (1) ;
}
void Run_PID_until_sensorback(int RUN_PID_speed,float RUN_PID_KP,float RUN_PID_KD,int direction_sensor,int function_read){
  
  do
  {
    int present_position = readline_2(function_read);
    int setpoint = ((PID_NumPin - 1) * 100) / 2;
    errors = present_position - setpoint;
	static int previous_error = 0;
    derivative = (errors - previous_error) ;
    output = RUN_PID_KP * errors  + RUN_PID_KD * derivative;
    
    int m1Speed = RUN_PID_speed + output ;
    int m2Speed = RUN_PID_speed - output;
    m1Speed = constrain(m1Speed, -80, 80);
    m2Speed = constrain(m2Speed, -80, 80);
    motor(1,m1Speed);
    motor(2,m2Speed);
    previous_error = errors;
    if(direction_sensor == 0){
        if(ADC(8)<700){
            break;
        }
    }
    else if(direction_sensor == 1){
         if(ADC(9)<700){
            break;
        }
    }
    else if(direction_sensor == 2){
         if(ADC(8)<700 and ADC(9)<700){
            break;
        }
    }
    else{
        if(ADC(8)<700 or ADC(9)<700){
            break;
        }
    }
    delay(1);
  }while (1) ;
}


void Run_PID_front_check_back(int check_state)
{
	display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(0));display.display();
	 while(1){
        Run_PID(40,0.5,5);
        if(Read_status_sensor(0) and (Read_status_sensor(5) or Read_status_sensor(6)) ){
           // display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(1));display.display(); 
           break;
        }
        if(Read_status_sensor(6) and (Read_status_sensor(0) or Read_status_sensor(1))){
            // display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(2));display.display(); 
            break;
        }
        else if(Read_status_sensor(0) and Read_status_sensor(1) and Read_status_sensor(2)and Read_status_sensor(3) and (check_state == 1 or check_state == 0)){
        	// display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(3));display.display(); 
        	break;
        }
        else if(Read_status_sensor(6) and Read_status_sensor(5) and Read_status_sensor(4) and Read_status_sensor(3) and (check_state == 2 or check_state == 0)){
        	// display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(4));display.display(); 
        	break;
        }
        else if(Read_sumValue_sensor() > 500){
        	// display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(5));display.display(); 
        	break;
        }
        
    }
    if(check_state == 0){
    	while(!Read_status_sensor(7) and !Read_status_sensor(8)){
	    	motor(1,20);motor(2,20);
	    }
	    delay(50);
	    ao();
	    //moveStraightSnapToNearest_andSensor(0, 20, 100, 0.5, 0, 0, 0);
	    //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 50, 0.5, 0, 0, 0);
    }
    else if(check_state == 1){
    	//moveStraightPID_Encoder(0, getContinuousYaw(), 40, 50, 0.5, 0, 0, 0);
    	//moveStraightSnapToNearest_andSensor(0, 20, 100, 0.5, 0, 0, 1);
    	while(!Read_status_sensor(7)){
	    	motor(1,20);motor(2,20);
	    }
	    delay(80);
	    ao();
    }
    else if(check_state == 2){
    	//moveStraightPID_Encoder(0, getContinuousYaw(), 40, 50, 0.5, 0, 0, 0);
    	//moveStraightSnapToNearest_andSensor(0, 20, 100, 0.5, 0, 0, 2);
    	while(!Read_status_sensor(8)){
	    	motor(1,20);motor(2,20);
	    }
	    delay(80);
	    ao();
    }


}
void move_SQ_4d(int direction_turn,int direction_exit){
   while(!Read_status_sensor(1) or !Read_status_sensor(5) ){
        Run_PID(40,0.5,5);
        if(Read_status_sensor(0) and (Read_status_sensor(5) or Read_status_sensor(6)) ){
            break;
        }
        if(Read_status_sensor(6) and (Read_status_sensor(0) or Read_status_sensor(1))){
            break;
        }
        else if(Read_sumValue_sensor() > 420){
        	break;
        }
        
    }
  while(!Read_status_sensor(7) and !Read_status_sensor(8)){
    	motor(1,20);motor(2,20);
    }
    delay(80);
    ao();
  //   delay(200);
  buzzer(500,100);
  if(direction_turn == 1){
    turnByAngle_fix(90);
    moveStraightSnapToNearest(0, 40, 500, 0.5, 0, 0, 0);
   //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(-90);
    if(direction_exit == 1){
    	//Run_PID_front_check_back(2);
    	moveStraightSnapToNearest(0, 40, 700, 0.5, 0, 0, 0);
         //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 700, 0.5, 0, 0, 0);
         turnByAngle_fix(90);

    }
    else if(direction_exit == 2){
        //Run_PID_until_sensorback(40, 0.2, 2, 0, 1);
        //Run_PID_front_check_back(1);
        moveStraightSnapToNearest(0, 40, 1300, 0.5, 0, 0, 0);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1300, 0.5, 0, 0, 0);
        turnByAngle_fix(-90);
        moveStraightSnapToNearest(0, 40, 550, 0.5, 0, 0, 0);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 550, 0.5, 0, 0, 0);
        turnByAngle_fix(90);
    }
  }
  if(direction_turn == 2){
    turnByAngle_fix(-90);
    moveStraightSnapToNearest(0, 40, 500, 0.5, 0, 0, 0);
    //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(90);
    if(direction_exit == 1){
    	//Run_PID_front_check_back(1);
    	moveStraightSnapToNearest(0, 40, 700, 0.5, 0, 0, 0);
         //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 700, 0.5, 0, 0, 0);
         turnByAngle_fix(-90);

    }
    else if(direction_exit == 2){
        //Run_PID_until_sensorback(40, 0.2, 2, 0, 1);
        moveStraightSnapToNearest(0, 40, 1300, 0.5, 0, 0, 0);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1300, 0.5, 0, 0, 0);
        //Run_PID_front_check_back(2);
        turnByAngle_fix(90);
        moveStraightSnapToNearest(0, 40, 550, 0.5, 0, 0, 0);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 550, 0.5, 0, 0, 0);
        turnByAngle_fix(-90);
    }
  }
}
void move_SQ_2d(int direction_turn){
  // Run_PID_until_sensorback(40, 0.2, 2, 2, 4);
  // ao();
  while(!Read_status_sensor(1) or !Read_status_sensor(5) ){
        Run_PID(35,0.5,5);
        if(ADC(8)<700 or ADC(9)<700){
            break;
        }
        if(Read_status_sensor(0) and (Read_status_sensor(5) or Read_status_sensor(6)) ){
            break;
        }
        if(Read_status_sensor(6) and (Read_status_sensor(0) or Read_status_sensor(1))){
            break;
        }
        else if(Read_sumValue_sensor() > 420){
        	break;
        }
        
    }
    buzzer(500, 100);
    while(!Read_status_sensor(7) and !Read_status_sensor(8)){
    	motor(1,20);motor(2,20);
    }
    delay(70);
    ao();
  buzzer(500,100);

  if(direction_turn == 1){
    turnByAngle_fix(90);
    moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(-90);
    Run_PID_front_check_back(1);
    //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1400, 0.5, 0, 0, 0);
    turnByAngle_fix(-90);
    moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(90);
    }
  else if(direction_turn == 2){
    turnByAngle_fix(-90);
    moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(90);
    //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1400, 0.5, 0, 0, 0);
    Run_PID_front_check_back(2);
    turnByAngle_fix(90);
    moveStraightPID_Encoder(0, getContinuousYaw(), 40, 500, 0.5, 0, 0, 0);
    turnByAngle_fix(-90);
  }
}


void move_diamond(int direction_turn, int direction_exit) {
    while(!Read_status_sensor(1) or !Read_status_sensor(5) ){
        Run_PID(35,0.5,5);
         if(Read_status_sensor(0) and Read_status_sensor(1) and Read_status_sensor(2)and Read_status_sensor(3)){
        	// display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(3));display.display(); 
        	break;
        }
        else if(Read_status_sensor(6) and Read_status_sensor(5) and Read_status_sensor(4) and Read_status_sensor(3) ){
        	// display.clearDisplay();display.setTextSize(3);drawString(0, 0, String(4));display.display(); 
        	break;
        }
        if(Read_status_sensor(0) and (Read_status_sensor(5) or Read_status_sensor(6)) ){
            break;
        }
        if(Read_status_sensor(6) and (Read_status_sensor(0) or Read_status_sensor(1))){
            break;
        }
        else if(Read_sumValue_sensor() > 420){
        	break;
        }
        
    }
    
    while(!Read_status_sensor(7) and !Read_status_sensor(8)){
    	motor(1,20);motor(2,20);
    }
    delay(50);
    ao();
    buzzer(500, 100);
    if (direction_turn == 1) {            // Turn RIGHT
        turnByAngle(45);
        Run_PID_front_check_back(1);
        //moveStraightPID_Encoder_andSensor(0, getContinuousYaw(), 40, 1000, 0.5, 0, 0, 0);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1000, 0.5, 0, 0, 0);

        if (direction_exit == 1) {
            turnByAngle_fix(45);
        } else if (direction_exit == 2) {

            turnByAngle(-90);
            Run_PID_front_check_back(1);
            //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1250, 0.5, 0, 0, 0);
            turnByAngle_fix(45);
        }
    }

    if (direction_turn == 2) {			// Turn LEFT
        turnByAngle(-45);
        //delay(3000);
        Run_PID_front_check_back(2);
        //moveStraightPID_Encoder_andSensor(0, getContinuousYaw(), 40, 1000, 0.5, 0, 0, 1);
        //delay(3000);
        //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1000, 0.5, 0, 0, 0);

        if (direction_exit == 1) {
            turnByAngle_fix(-45);
        } else if (direction_exit == 2) {
            turnByAngle(90);
            Run_PID_front_check_back(2);
            //moveStraightPID_Encoder(0, getContinuousYaw(), 40, 1250, 0.5, 0, 0, 0);
            turnByAngle_fix(-45);
        }
    }
}
void move_cross(int direction_turn){
	// while(!Read_status_sensor(1) or !Read_status_sensor(5) ){
 //        Run_PID(40,0.5,5);
 //        if(Read_status_sensor(0) and (Read_status_sensor(5) or Read_status_sensor(6)) ){
 //            break;
 //        }
 //        if(Read_status_sensor(6) and (Read_status_sensor(0) or Read_status_sensor(1))){
 //            break;
 //        }
 //        else if(Read_sumValue_sensor() > 420){
 //        	break;
 //        }
        
 //    }
 //    Run_PID_until_sensorback(40, 0.2, 2, 2, 4);
 //    ao();
 //    buzzer(500,200);
	Run_PID_front_check_back(0);
  if(direction_turn == 0){
    turnByAngle_fix(-90);
    Run_PID_until_encoder(40, 0.5, 2, 300,1);
   
  }
  else if(direction_turn == 1){
    Run_PID_until_encoder(40, 0.5, 2, 600,1);
   
  }
  else if(direction_turn == 2){
    turnByAngle_fix(90);
    Run_PID_until_encoder(40, 0.5, 2, 300,1);
   
  }
}
void move_tri(int direction_turn){
	//Run_PID_until_frontSensor(40,0.5,2,370,4);
	// motor(1,30);
	// motor(2,30);
	// delay(100);
	Run_PID_front_check_back(0);
	if(direction_turn == 0){
		ao();
		turnByAngle_fix(-90);
		Run_PID_until_encoder(40, 0.5, 2, 300,0);
		//moveStraightPID_Encoder(0, getContinuousYaw(), 40, 300, 0.5, 0, 0, 0);
	}
	else if(direction_turn == 1){
		ao();
		turnByAngle_fix(90);
		Run_PID_until_encoder(40, 0.5, 2, 300,0);
		//moveStraightPID_Encoder(0, getContinuousYaw(), 40, 300, 0.5, 0, 0, 0);
	}
	else{
		Run_PID_until_encoder(40, 0.5, 2, 600,0);
		//moveStraightPID_Encoder(0, getContinuousYaw(), 40, 600, 0.5, 0, 0, 0);
	}
    //moveStraightSnapToNearest(0,20, 100,0.5,0,0.1,0);
}





void move_number2(int direction_turn){
  if(direction_turn == 1){
    Run_PID_until_encoder(30,0.5,5,3850,2);
  }
  else if(direction_turn == 2){
    Run_PID_until_encoder(30,0.3,5,4000,3);
  }
  ao();
  delay(100);
}
void move_start(){
    MoveStraightDirection_Encoder(0, 0, 50, 1100, 0.2, 0, 0.1, 0);
}
void move_straight(){
    Run_PID_until_encoder(50, 0.5, 2, 2300,4);
}
void move_back(){
    //Run_PID_until_encoder(50, 0.5, 2, 5000,4);
    moveStraightSnapToNearest(1,30, 400,0.5,0,0.1,0);
}
void move_original_angle(int direction,int speed,int pluse){
    moveStraightSnapToNearest(direction,speed, pluse,0.5,0,0.1,0);
}
void move_chopsticks(){
    servoRun(4, 90);
    delay(100);
    moveStraightSnapToNearest(0,20, 4100,0.5,0,0.1,0);
    ao();
  	servoRun(4, 132);
  	delay(100);
  	motor(1, -10);
  	motor(2, -10);
  	delay(700);
}
void move_snake(){
    //Run_PID_until_encoder(25, 0.5, 5, 3300,0);
    Run_PID_until_encoder(25, 0.5, 2, 3300,0);
}
void move_triangle(){
    
    Run_PID_until_encoder(20, 0.5, 2, 2800,0);
}
void move_Sixsax(){
    //Run_PID_until_encoder(50, 0.5, 2, 5000,4);
    Run_PID_until_encoder(35, 0.5, 5, 3300,0);
}
void move_S(){
    Run_PID_until_encoder(35,0.4,2,4500,0);
    //Run_PID_until_encoder(35,0.4,5,4500,0);
}
void move_kong(){
    
    Run_PID_until_encoder(35,0.6,5,2100,0);
}
void Turn_ultil_check_Sensor(int direction_turn){
	if(direction_turn == 2){
		motor(1,-20);
        motor(2,-20);
        delay(10);
	}
    else if(direction_turn == 0){
        motor(1,-50);
        motor(2,50);
    }
    else{
        motor(1,50);
        motor(2,-50);
    }
}
