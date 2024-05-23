#include "components.hpp"


void TB::clockGeneration()
{
	while (true)
	{
		clk = SC_LOGIC_0;
		wait(10, SC_NS);

		clk = SC_LOGIC_1;
		wait(10, SC_NS);
	}
}
void TB::resetAssertion()
{
	while (true)
	{
        cout << "\nNEW RESET\n";
        start = SC_LOGIC_0;
		rst = SC_LOGIC_1;

		wait(15, SC_NS);
		rst = SC_LOGIC_0;
        start = SC_LOGIC_1;
        
        wait(15, SC_NS);
        start = SC_LOGIC_0;
		wait();
	}
}

void Comparator::evl()
{
    tmpA = A;
    tmpB = B;

    lt = (tmpA < tmpB) ? SC_LOGIC_1 : SC_LOGIC_0;
}

void MUX2to1_8::evl() {
    if (sel == '0')
        Y = A;
    else if (sel == '1')
        Y = B;
}

void MUX3to1_8::evl()
{
    if (sel.read() == "00") {
        Y = A;
    } else if (sel.read() == "01") {
        Y = B;
    } else if (sel.read() == "10") {
        Y = C;
    }
}

void MUX2to1_16::evl()
{
    if (sel == '0')
        Y = A;
    else if (sel == '1')
        Y = B;
}

void SSC::report()
{
    wait(50, SC_NS);
    for (int i = 0; i<300; i++) {
    //     cout << "start: " << start << endl;
    //     cout << "done: " << done << endl;

        // cout << "count1: " << count1.read().to_uint() << endl;
        // cout << "count2: " << count2.read().to_uint() << endl;
        // cout << "lt: " << lt << endl << endl;
        wait(20, SC_NS);
    }
    wait();
}


void Controller::combinational()
{
    // cout << "\n**COMBINATIONAL**\n";
    done = SC_LOGIC_0;
    ldTmp = SC_LOGIC_0;
    ldCnt1 = SC_LOGIC_0;
    ldCnt2 = SC_LOGIC_0;
    enCnt1 = SC_LOGIC_0;
    enCnt2 = SC_LOGIC_0;
    rfWrEn = SC_LOGIC_0;
    rfRdEn = SC_LOGIC_0;
    ldMinAdr = SC_LOGIC_0;
    ldMinVal = SC_LOGIC_0;
    selMin = SC_LOGIC_0;
    selAdr = "00";

    switch (pState)
    {
    case 0: //IDLE
        nState = (start == "1") ? 1 : 0;
        break;
    
    case 1: //load counter2        
        ldCnt2 = SC_LOGIC_1;
        rfRdEn = SC_LOGIC_1;
        selAdr = "00";
        
        nState = 2;
        break;
    
    case 2: //init registers
        ldMinVal = SC_LOGIC_1;
        ldMinAdr = SC_LOGIC_1;
        ldTmp = SC_LOGIC_1;
        selMin = SC_LOGIC_0;
        
        nState = 3;
        break;

    case 3: // inner loop >> update counter2
        enCnt2 = SC_LOGIC_1;

        nState = (count2.read().to_uint() < 255) ? 10 : 6;
        break;

    case 10:
        selAdr = "01";
        rfRdEn = SC_LOGIC_1;
        nState = 4;
        break;

    case 4: // compare    
        nState = (lt == '1') ? 5 : 3;
        break;

    case 5: // update MinVal and MinAdr
        ldMinAdr = SC_LOGIC_1;
        ldMinVal = SC_LOGIC_1;

        nState = 3;
        break;

    case 6: // Swap1
        selAdr = "10";
        rfWrEn = SC_LOGIC_1;
        ldTmp = SC_LOGIC_1;
        selMin = SC_LOGIC_1;
        
        nState = 7;
        break;


    case 7: //Swap2
        selAdr = "00";
        rfWrEn = SC_LOGIC_1;

        nState = 8; 
        break;
    
    case 8: // Outer loop >> update counter1
        enCnt1 = SC_LOGIC_1;

        nState = count1.read().to_uint() < 255 ? 1 : 9;
        break;

    case 9:
        done = SC_LOGIC_1;

        nState = 0;
        break;

    default:
        cout << "\ncoming to default!!\n";
        nState = 0;
    }

    // cout << "PS: " << pState << " | " << "NS: " << nState << endl;
}

void Controller::sequential()
{
    while (true) {
        // cout << "here at control seq part\n";
        if (rst.event() && rst == '1') {
            pState = 0;
        } else if (clk.event() && clk == '1') {
            pState = nState;
        }
        wait();
    }
}
