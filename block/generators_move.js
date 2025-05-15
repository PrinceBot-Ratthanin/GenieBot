module.exports = function(Blockly){
  'use strict';

Blockly.JavaScript['Move_To_partA'] = function(block) {  
  var code = 'move_snake();\n';
  return code;
};
Blockly.JavaScript['Move_To_partB'] = function(block) {  
  var code = 'move_Sixsax();\n';
  return code;
};

Blockly.JavaScript['Move_To_partD'] = function(block) {  
  var code = 'move_S();\n';
  return code;
};
Blockly.JavaScript['Move_To_partH'] = function(block) {  
  var dropdown_direction = block.getFieldValue('direction');
  var code = 'move_cross('+dropdown_direction+');\n';
  return code;
};
Blockly.JavaScript['Move_start'] = function(block) {  
  var code = 'move_start();\n';
  return code;
};
Blockly.JavaScript["Move_To_partC"] = function(block) {
  // ดึงค่าที่ผู้ใช้เลือกจาก dropdown
  var dropdown_value = block.getFieldValue(`IMG_SELECT_1`);

  // สร้างโค้ดตามค่าที่เลือก
  var code = "";
  if (dropdown_value === "icon1") {
    code = "move_number2(1);\n";
  } else if (dropdown_value === "icon2") {
    code = "move_number2(2);\n";
  } else {
    code = "// ไม่พบตัวเลือกที่ตรง\n";
  }

  return code;
};
Blockly.JavaScript['Move_To_partE'] = function(block) {  
  var dropdown_direction = block.getFieldValue('direction');

  var code ='';
  if(dropdown_direction == '0'){code +=  'move_SQ_4d(2,1);\n';}
  if(dropdown_direction == '1'){code +=  'move_SQ_4d(1,1);\n';}
  if(dropdown_direction == '2'){code +=  'move_SQ_4d(2,2);\n';}
  if(dropdown_direction == '3'){code +=  'move_SQ_4d(1,2);\n';}
  return code;
};
Blockly.JavaScript['Move_To_partF'] = function(block) {  
  var dropdown_direction = block.getFieldValue('direction');

  var code ='';
  if(dropdown_direction == '0'){code +=  'move_diamond(2,1);\n';}
  if(dropdown_direction == '1'){code +=  'move_diamond(1,1);\n';}
  if(dropdown_direction == '2'){code +=  'move_diamond(2,2);\n';}
  if(dropdown_direction == '3'){code +=  'move_diamond(1,2);\n';}
  return code;
};
Blockly.JavaScript['Move_To_partG'] = function(block) {  
  var dropdown_direction = block.getFieldValue('direction');
  var code = 'move_SQ_2d('+dropdown_direction+');\n';
  return code;
};
Blockly.JavaScript['Move_To_partI'] = function(block) {  
  var code = 'move_straight();\n';
  return code;
};
Blockly.JavaScript['Move_To_partJ'] = function(block) {  
  var code = 'move_triangle();\n';
  return code;
};
Blockly.JavaScript['Move_To_partK'] = function(block) {  
  var dropdown_direction = block.getFieldValue('direction');

  var code ='move_tri('+dropdown_direction+');';
  return code;
};
Blockly.JavaScript['Move_To_partL'] = function(block) {  

  var code ='move_kong();';
  return code;
};

Blockly.JavaScript['Move_back'] = function(block) {  
  var code = 'move_back();\n';
  return code;
};
Blockly.JavaScript['Move_chopsticks'] = function(block) {  
  var code = 'move_chopsticks();\n';
  return code;
};
Blockly.JavaScript['Turn_ultil_check_Sensor'] = function(block) {
  var dropdown_dir = block.getFieldValue('direction');
  var value_s2 = Blockly.JavaScript.valueToCode(block, 'S2', Blockly.JavaScript.ORDER_ATOMIC) || '0';


  var code = '';
  code += 'while(!('+value_s2+')){\n';
  code += 'Turn_ultil_check_Sensor('+dropdown_dir+');\n';
  code += '}ao();';
  if(dropdown_dir == '2'){
    code += 'motor(1,20);motor(2,20);delay(150);ao();';
  }
  return code;
};
Blockly.JavaScript['move_original_angle'] = function(block) {
  var dropdown_dir = block.getFieldValue('direction');
  var value_s0 = Blockly.JavaScript.valueToCode(block, 'S0', Blockly.JavaScript.ORDER_ATOMIC) || '0';
  var value_s1 = Blockly.JavaScript.valueToCode(block, 'S1', Blockly.JavaScript.ORDER_ATOMIC) || '0';


  var code = 'move_original_angle('+dropdown_dir+','+value_s0+','+value_s1+');';
  return code;
};
Blockly.JavaScript['move_PID_Encoder'] = function(block) {
  var value_s0 = Blockly.JavaScript.valueToCode(block, 'S0', Blockly.JavaScript.ORDER_ATOMIC) || '0';
  var value_s1 = Blockly.JavaScript.valueToCode(block, 'S1', Blockly.JavaScript.ORDER_ATOMIC) || '0';


  var code = 'Run_PID_until_encoder('+value_s0+',0.5,5,'+value_s1+',0);';
  return code;
};
Blockly.JavaScript['move_turn_by_angle'] = function(block) {
  var dropdown_dir = block.getFieldValue('direction');
  var dropdown_angle = block.getFieldValue('angle');

  var code = 'turnByAngle_fix('+dropdown_dir+''+dropdown_angle+');';
  return code;
};

Blockly.JavaScript['move_PID_until_checkSum'] = function(block) {
  var value_s0 = Blockly.JavaScript.valueToCode(block, 'S0', Blockly.JavaScript.ORDER_ATOMIC) || '0';
  var value_s1 = Blockly.JavaScript.valueToCode(block, 'S1', Blockly.JavaScript.ORDER_ATOMIC) || '0';

  var code = 'Run_PID_until_frontSensor('+value_s0+',0.5,2,'+value_s1+',0);';
  return code;
};

Blockly.JavaScript['move_PID_until_sensor'] = function(block) {
  var value_s0 = Blockly.JavaScript.valueToCode(block, 'S0', Blockly.JavaScript.ORDER_ATOMIC) || '0';
  var value_s1 = Blockly.JavaScript.valueToCode(block, 'S1', Blockly.JavaScript.ORDER_ATOMIC) || '0';

  var code = 'Run_PID_until_can('+value_s0+',0.3,2,'+value_s1+',0);';
  return code;
};


}