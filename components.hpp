#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <iostream>
#include <systemc.h>

#define MEM_INPUT_FILE "mem.txt"
#define MEM_OUTPUT_FILE "output_sort.txt"

using namespace std;

template<int bits>
class CounterNbit: public sc_module
{
public:
    sc_in<sc_logic> clk, rst, cntEn, loadEn;
    sc_in<sc_lv<bits>> loadData;
    sc_out<sc_lv<bits>> countOut;

    sc_uint<bits> temp;

    SC_HAS_PROCESS(CounterNbit);
    CounterNbit(sc_module_name);

    void eval();
};

template<int bits>
CounterNbit<bits>::CounterNbit(sc_module_name)
{
    temp = 0;

    SC_THREAD(eval);
    sensitive << clk << rst;
}

template<int bits>
void CounterNbit<bits>::eval()
{
    while (true) {
        if (rst.event() && rst == "1") {
            cout << "counter resetting...\n";
            temp = 0;
        } else if (clk.event() && clk == '1') {
            if (loadEn == '1') {
                // cout << "counter loading happended\n";
                temp = loadData;
            } else if (cntEn == '1') {
                temp += 1;
            }
        }
        countOut = temp;
        wait();
    }
}

template<int addressBits, int wordSize>
class RegisterFile : public sc_module
{
public:
	sc_in<sc_logic> clk, rst, read, write;
	sc_in<sc_lv<addressBits> > Addr;
	sc_in<sc_lv<wordSize> > Din;
	sc_out<sc_lv<wordSize> > Dout;

	int rows;
	sc_uint <wordSize> *mem;

	void meminit();
	void resetting();
	void memread();
	void memwrite();
	void memdump();

	SC_HAS_PROCESS(RegisterFile);
	RegisterFile(sc_module_name);
};

template<int addressBits, int wordSize>
RegisterFile<addressBits, wordSize>::RegisterFile(sc_module_name) 
{
	rows = int(pow(2.0, addressBits));
	mem = new sc_uint<wordSize>[rows];

	SC_THREAD(meminit);
    
    // SC_THREAD(resetting);
    // sensitive << rst.pos();
	
    SC_METHOD(memread);
	sensitive << Addr << read;
	
    SC_THREAD(memwrite);
	sensitive << clk;
	
    SC_METHOD(memdump);
    sensitive << clk;
}

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::meminit()
{
    cout << "\nMEMORY INIT STARTED ***\n";

    ifstream in(MEM_INPUT_FILE);
    int i;
    sc_lv<wordSize> data;

    for (i = 0; i < rows; i++) {
        in >> data;
        mem[i] = data;
    }
}

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::memwrite()
{
	sc_uint<addressBits> ad;
    while (true)
    {
        if (clk.event() && clk == '1') {
            if (write == '1') {
                ad = Addr; 
                mem[ad] = Din;
                cout << "writing @" << ad << ": " << Din.read() << "( " <<  mem[ad] << " )" << endl;
            }
        }
        wait();
    }
}

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::resetting()
{
	sc_uint<addressBits> ad;
    while (true)
    {
        if (rst == '1') {
            cout << "*** RESETTING MEMORY ***\n";
            for (int i = 0; i < rows; i++)
                mem[i] = 0;
        }
        wait();
    }
}

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::memread()
{
	sc_uint<addressBits> ad;
    if (read == '1') { // not dependant to any clock events!
        ad = Addr; 
        Dout = mem[ad];
        // cout << "\nmem read @ " << ad << ": " << mem[ad] << endl;
    }
}

template<int addressBits, int wordSize>
void RegisterFile<addressBits, wordSize>::memdump()
{
	int i;
	sc_lv<wordSize> data;
	ofstream out(MEM_OUTPUT_FILE);

	for (i = 0; i < rows; i++) {
		data = mem[i];
		out << data << "\n";
	}
}

template<int bits>
class RegisterNbit: public sc_module
{
public:
    sc_in<sc_logic> clk, rst;
    sc_in<sc_logic> loadEnable;
    sc_in<sc_lv<bits>> dataIn;

    sc_out<sc_lv<bits>> dataOut;

    sc_uint<bits> temp;

    SC_HAS_PROCESS(RegisterNbit);
    RegisterNbit(sc_module_name);

    void registering();
    const char* getName() {
        return sc_get_current_process_b()->get_parent_object()->basename();
    }
};

template<int bits>
RegisterNbit<bits>::RegisterNbit(sc_module_name)
{
    temp = 0;

    SC_THREAD(registering);
    sensitive << clk << rst;
}

template<int bits>
void RegisterNbit<bits>::registering()
{
    while (true) {
        // cout << "\n****RUNNING REG PART ***\n";
        if (rst.event() && rst == '1') {
            cout << getName() << ": ";
            // cout << "reg reset happen" << endl;
            temp = 0;
        } else if (clk.event() && clk == '1') {
            if (loadEnable == '1') {
                // cout << getName() << ": loaded with value of " << dataIn.read().to_uint() << endl;
                // cout << to_string(sc_module_name) << " loading happened\n";
                temp = dataIn;
            }
        }

        dataOut = temp;
        wait();
    }
}

SC_MODULE(Comparator) {
    sc_in<sc_lv<16> > A, B;
    sc_out<sc_logic> lt;

    sc_uint<16> tmpA, tmpB;

    SC_CTOR(Comparator) {
        SC_METHOD(evl);
        sensitive << A << B;
    }

    void evl();
};

SC_MODULE(MUX2to1_8) {
    sc_in<sc_lv<8>> A, B;
    sc_in<sc_logic> sel;
    sc_out<sc_lv<8>> Y;

    SC_CTOR(MUX2to1_8) {
        SC_METHOD(evl);
        sensitive << A << B << sel;
    }
    
    void evl();
};

SC_MODULE(MUX3to1_8) {
    sc_in<sc_lv<8>> A, B, C;
    sc_in<sc_lv<2>> sel;
    sc_out<sc_lv<8>> Y;

    SC_CTOR(MUX3to1_8) {
        SC_METHOD(evl);
        sensitive << A << B << C << sel;
    }
    
    void evl();
};

SC_MODULE(MUX2to1_16) {
    sc_in<sc_lv<16>> A, B;
    sc_in<sc_logic> sel;
    sc_out<sc_lv<16>> Y;

    SC_CTOR(MUX2to1_16) {
        SC_METHOD(evl);
        sensitive << A << B << sel;
    }
    void evl();
};

SC_MODULE(Datapath)
{
    sc_in<sc_logic> clk, rst, ldTmp, ldCnt1, ldCnt2, enCnt1, enCnt2, rfWrEn, rfRdEn, selMin, ldMinAdr, ldMinVal;
    sc_in<sc_lv<8>> cnt1LoadData;
    sc_in<sc_lv<2>> selAdr;
    sc_out<sc_logic> lt;
    sc_out<sc_lv<8>> count1, count2;
    
    sc_signal<sc_lv<8>> minAdrOut, address;
    sc_signal<sc_lv<16>> minValOut, readData, tempRegIn, writeData;

    CounterNbit<8> *counter1, *counter2;
    RegisterNbit<16> *tempReg, *minValReg;
    RegisterNbit<8> *minAdrReg;
    RegisterFile<8, 16> *rf;
    MUX3to1_8 *muxSelAdr;
    MUX2to1_16 *muxSelMin;
    Comparator *cmp;


    SC_CTOR(Datapath) {
        counter1 = new CounterNbit<8>("counter1");
        counter1->clk(clk);
        counter1->rst(rst);
        counter1->loadEn(ldCnt1);
        counter1->loadData(cnt1LoadData);
        counter1->cntEn(enCnt1);
        counter1->countOut(count1);

        counter2 = new CounterNbit<8>("counter2");
        counter2->clk(clk);
        counter2->rst(rst);
        counter2->loadEn(ldCnt2);
        counter2->loadData(count1);
        counter2->cntEn(enCnt2);
        counter2->countOut(count2);
        
        tempReg = new RegisterNbit<16>("temp_register");
        tempReg->clk(clk);
        tempReg->rst(rst);
        tempReg->loadEnable(ldTmp);
        tempReg->dataIn(tempRegIn);
        tempReg->dataOut(writeData);

        minValReg = new RegisterNbit<16>("minVal_register");
        minValReg->clk(clk);
        minValReg->rst(rst);
        minValReg->loadEnable(ldMinVal);
        minValReg->dataIn(readData);
        minValReg->dataOut(minValOut);

        minAdrReg = new RegisterNbit<8>("minAdr_register");
        minAdrReg->clk(clk);
        minAdrReg->rst(rst);
        minAdrReg->loadEnable(ldMinAdr);
        minAdrReg->dataIn(count2);
        minAdrReg->dataOut(minAdrOut);


        rf = new RegisterFile<8, 16>("RegisterFile");
        rf->clk(clk);
        rf->rst(rst);
        rf->Addr(address);
        rf->Din(writeData);
        rf->Dout(readData);
        rf->read(rfRdEn);
        rf->write(rfWrEn);

        muxSelMin = new MUX2to1_16("select_temp_input_mux");
        muxSelMin->A(readData);
        muxSelMin->B(minValOut);
        muxSelMin->Y(tempRegIn);
        muxSelMin->sel(selMin);

        muxSelAdr = new MUX3to1_8("address_select_mux");
        muxSelAdr->A(count1);
        muxSelAdr->B(count2);
        muxSelAdr->C(minAdrOut);
        muxSelAdr->Y(address);
        muxSelAdr->sel(selAdr);

        cmp = new Comparator("comparatorInstance");
        cmp->A(readData);
        cmp->B(minValOut);
        cmp->lt(lt);
    }
};

SC_MODULE(Controller) {
    sc_in<sc_logic> start, clk, rst, lt;
    sc_in<sc_lv<8>> count1, count2;

    sc_out<sc_logic> ldTmp, ldCnt1, ldCnt2, enCnt1, enCnt2, rfWrEn, rfRdEn, selMin, ldMinAdr, ldMinVal, done;
    sc_out<sc_lv<2>> selAdr;

    sc_signal<int> pState, nState;

    SC_CTOR(Controller) {
        SC_METHOD(combinational);
        sensitive << pState << start << lt << count1 << count2;

        SC_THREAD(sequential);
        sensitive << clk << rst;
    }

    void combinational();
    void sequential();
};

SC_MODULE(SSC)
{
    sc_in<sc_logic> clk, rst, start;
    sc_out<sc_logic> done;

    sc_signal<sc_logic> ldTmp, ldCnt1, ldCnt2, enCnt1, enCnt2, rfWrEn, rfRdEn, selMin, ldMinAdr, ldMinVal, lt;
    sc_signal<sc_lv<8>> count1LoadData, count1, count2;
    sc_signal<sc_lv<2>> selAdr;

    Datapath* dp;
    Controller* ct;

    SC_CTOR(SSC) {
        count1LoadData = "00000000";
        selAdr = "00";
        
        dp = new Datapath("DataPathInstance");
        dp->clk(clk);
        dp->rst(rst);
        dp->ldCnt1(ldCnt1);
        dp->ldCnt2(ldCnt2);
        dp->enCnt1(enCnt1);
        dp->enCnt2(enCnt2);
        dp->rfRdEn(rfRdEn);
        dp->rfWrEn(rfWrEn);
        dp->selMin(selMin);
        dp->selAdr(selAdr);
        dp->ldMinAdr(ldMinAdr);
        dp->ldMinVal(ldMinVal);
        dp->ldTmp(ldTmp);
        dp->lt(lt);
        dp->count1(count1);
        dp->count2(count2);
        dp->cnt1LoadData(count1LoadData);

        ct = new Controller("ControllerInstance"); 
        ct->clk(clk);
        ct->rst(rst);
        ct->start(start);
        ct->done(done);
        ct->ldCnt1(ldCnt1);
        ct->ldCnt2(ldCnt2);
        ct->enCnt1(enCnt1);
        ct->enCnt2(enCnt2);
        ct->rfRdEn(rfRdEn);
        ct->rfWrEn(rfWrEn);
        ct->selMin(selMin);
        ct->selAdr(selAdr);
        ct->ldMinAdr(ldMinAdr);
        ct->ldMinVal(ldMinVal);
        ct->ldTmp(ldTmp);
        ct->lt(lt);
        ct->count1(count1);
        ct->count2(count2);

        // SC_THREAD(report);
    }

    void report();

};

SC_MODULE(TB)
{
    sc_signal<sc_logic> clk, rst, start, done;

    SC_CTOR(TB) {
        SSC* UUT = new SSC("Unit_under_test");
        UUT->clk(clk);
        UUT->rst(rst);
        UUT->start(start);
        UUT->done(done);

        
        SC_THREAD(clockGeneration);
        SC_THREAD(resetAssertion);
        SC_THREAD(timing);

        sc_trace_file* VCDFile;
        VCDFile = sc_create_vcd_trace_file("wave_result");
        sc_trace(VCDFile, clk, "clk");
        sc_trace(VCDFile, rst, "rst");
        sc_trace(VCDFile, start, "start");
        sc_trace(VCDFile, done, "done");
	}

	void clockGeneration();
	void resetAssertion();

    void timing() {
        while (done.read() != '1') {
            wait(10, SC_NS);
        }
        cout << endl << "at time: " << sc_time_stamp() << " | done = " << done.read() << endl;
    }

};


#endif
