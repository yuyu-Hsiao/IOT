//超音波測距模組HC-SR04
const byte trigPin=32; //超音波測距的 觸發腳
const byte echoPin=33; //超音波測距的 回應腳

void hcsr04_setup(){
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

//超音波測距函式  
unsigned long sr04() { 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH);
}
 
int dis_cm(){
  return (sr04()*0.0346675)/2;
}