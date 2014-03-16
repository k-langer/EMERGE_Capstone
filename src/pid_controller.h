#include <assert.h>
#include <iostream>
#include <vector>

using namespace std;

#define KP 0.3
#define KI 0.004
#define KD 0.25
#define STEPCOUNT 6 // Set umber of steps till goal is reached

class PID 
{
public:
	PID(int mMax, int mMin);
	int pidP();
	int pidI(int numIterations);
	int pidD(int numIterations);
	int ticksPerMSec(int time); //time in millisecs
	void setDebug(bool enable);
    void setAnalysisMode(bool enable);
	vector <int> getSteps(int curr, int goal);

private:
	int pidSteps; // Number of PID iterations to goal
	int maxSpeed; // Ratio of (goal-curr)/time in ticks per mSec
	int curr = 0;
	int goal;
	int motormax;
	int motormin;
	vector <int> diffs;
	void setMaxMotor(int max);
	void setMinMotor(int min);
    void initParam(int currentPosition, int goalPosition);
	bool debug = false;
    bool analysis = false;
	vector <int> pid_val;
    vector<int>curr_val;
    bool reverseDirection = false;
};

void PID::initParam(int currentPosition, int goalPosition)
{
        curr = currentPosition;
        goal = goalPosition;
        diffs.clear();
        if(curr > goal) reverseDirection = true;
        else reverseDirection = false;
}
void PID::setMaxMotor(int max)
{
	motormax = max;
}

void PID::setMinMotor(int min)
{
	motormin = min;
}

PID::PID(int mMax, int mMin)
{
	setMaxMotor(mMax);
	setMinMotor(mMin);
	for (int i = 0; i < STEPCOUNT; i++){
		pid_val.push_back(-1);
        curr_val.push_back(-1);
    }
}
vector <int> PID::getSteps(int curr1, int goal1)
{
    //initialize curr and goal
    initParam(curr1, goal1);
    // Current value is in proper range
    cout << curr << " " << motormin <<  " " << motormax << endl;
    cout << goal << " " << motormin <<  " " << motormax << endl;
	assert(curr >= motormin && curr <= motormax);
    // Goal value is in proper range
	assert(goal >= motormin && goal <= motormax);
	if (analysis) {
        cout << "Current: " << curr1
             << "; Goal: "  <<goal1
             << endl;
        cout << "--------------"<<endl;
    }
	for (int i = 0; i < STEPCOUNT; i++)
	{
        if(analysis){
            cout << curr << endl;
        }
        if(reverseDirection)
        {
		    pid_val[i]= pidP() -  pidI(i) - pidD(i);
            curr = curr + pid_val[i];
            if (curr > motormax) curr = motormax;
            if (curr < goal) curr = goal;
        }
        else
        {
		    pid_val[i]= pidP()+ pidI(i) + pidD(i);
            curr = curr + pid_val[i];
            if (curr > goal) curr = goal;
            if (curr < motormin) curr = motormin;
        }
        
        curr_val[i] = curr;
        
		if (debug)
		{
			cout << "Goal: " << goal 
                 << " Curr: " << curr
                 << " Ticks/Sec: " << pid_val[i]
                 << endl;
		}
	}
    if(analysis){
        cout << curr << endl;
    }
	return curr_val;
}

int PID::pidP() // Difference between current value and desired value
{
	int dist = goal - curr;
	diffs.push_back(abs(dist));
	return (int) dist * KP;
}

int PID::pidI(int numIterations) // Sum of differences over time
{
	int sum = 0;
	for (int i = 0; i < numIterations; i++)
		sum = sum + diffs[i];
	return (int) sum * KI;
}

int PID::pidD(int numIterations) // Rate of change between the sampled differences
{
	if (numIterations >= 1)
	{
		int sum = abs(diffs[numIterations] - diffs[numIterations - 1]);
		return (int)sum * KD;
	}
	else return 0;
}

int PID::ticksPerMSec(int time)
{
	return (goal - curr) / time; 
}

void PID::setDebug(bool enable)
{
	debug = enable;
}
void PID::setAnalysisMode(bool enable){
    analysis = enable;
}

/*int main(){
    int testGoal = 800;
    int testCurr = 300;
    int MAX = 1024;
    int MIN = 200;
    PID test(MAX, MIN);
//    test.setDebug(true);
    test.setAnalysisMode(true);
    cout << "Current:" << testCurr << "; Goal:" << testGoal <<endl;
    test.getSteps(testCurr, testGoal);
    testGoal = 300;
    testCurr = 800;
    cout << "Current:" << testCurr << "; Goal:" << testGoal <<endl;
    test.getSteps(testCurr, testGoal);
}*/


