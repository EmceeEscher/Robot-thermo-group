#ifndef motor_h
#define motor_h

class motorClass
{
public:
    //consturctor
    motorClass();
    
    //member functions
    void stop(int motorVal);
    void stop_all();
    void speed(int motorVal, int speedval);
    //		void detach(int motorVal) ;
    
private:
    int _isvalid ;
};

#endif
