class CellStateMachine
{
public:
	CellStateMachine()
	{}


	void StateZero(); //normal
	void StateOne(); //failed once
	void StateTwo(); //failed twice - reset modem
	void StateThree(); //failed after reset, now only try once an hour.
	
	

	unsigned int m_CurrentState;
	//
}