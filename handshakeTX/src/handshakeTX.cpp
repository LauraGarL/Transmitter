//============================================================================
// Name        : handshakeTX.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "tx.h"
#include <iostream>
#include "systemc.h"
using namespace std;

int sc_main(int argc, char* argv[]) {
    sc_signal<bool> i_clock;
    sc_signal<bool> i_reset;
    sc_signal<bool> i_packetReady;
    sc_signal<bool> i_OnOff;
    sc_signal<bool>	i_fifo_empty;	//Input: Indicates that the FIFO is empty
    sc_signal<sc_uint<DATA_WIDTH>> 	i_fifo_dataOut;	//Input: Data_out from FIFO
    sc_signal<bool> o_Req;
    sc_signal<bool> o_SoP;
    sc_signal<bool> o_fifo_pop;

    TX transmitter("transmitter");
    transmitter.i_clock(i_clock);
    transmitter.i_reset(i_reset);
    transmitter.i_packetReady(i_packetReady);
    transmitter.i_OnOff(i_OnOff);
    transmitter.i_fifo_dataOut(i_fifo_dataOut);
    transmitter.i_fifo_empty(i_fifo_empty);
    transmitter.o_Req(o_Req);
    transmitter.o_SoP(o_SoP);
    transmitter.o_fifo_pop(o_fifo_pop);

    sc_start(10,SC_NS);	//El entero nos dice cuántas unidades de tiempo se va a ejecutar. Es un método necesario para comenzar una simulación.

    		sc_trace_file *wf = sc_create_vcd_trace_file("vcd_myTX");

    		sc_trace(wf, i_clock, "i_clock");
    		sc_trace(wf, i_reset, "i_reset");
    		sc_trace(wf, i_packetReady, "i_packetReady");
    		sc_trace(wf, i_OnOff, "i_OnOff");
    		sc_trace(wf, i_fifo_empty,"i_fifo_empty");
    		sc_trace(wf, i_fifo_dataOut,"i_fifo_dataOut");
    		sc_trace(wf, o_Req, "o_Req");
    		sc_trace(wf, o_SoP, "o_SoP");
    		sc_trace(wf, o_fifo_pop, "o_fifo_pop");
    		sc_trace(wf, transmitter.state, "state");
    		sc_trace(wf, transmitter.next_state, "next_state");
    		sc_trace(wf, transmitter.count, "count");

    		i_reset = 0;
    		i_packetReady = 0;
    		i_OnOff = 0;
    		i_fifo_empty = 0;
    		i_fifo_dataOut = 0;

    		for(int i=0; i<2; i++){
    					i_clock = 0;
    					sc_start(10,SC_NS);
    					i_clock = 1;
    					sc_start(10,SC_NS);
    				}

    				i_packetReady = 0;
    		    	i_OnOff = 0;
    				i_reset = 1; //Assert the reset
    				i_fifo_empty = 1;
    				i_fifo_dataOut = 0;

    				cout << "@" << sc_time_stamp() <<"\t Asserting reset ...TX\n" << endl;

    				for (int i=0; i<2; i++){
    					i_clock = 0;
    					sc_start(10,SC_NS);
    					i_clock = 1;
    					sc_start(10,SC_NS);
    				}

    				i_reset = 0; //De-Assert the reset

    				cout << "@" << sc_time_stamp() <<"\t De-Asserting reset\n" << endl;

    				cout << "@" << sc_time_stamp() <<"\t All configurations:\n" << endl; //Quiero poner todas las combinaciones de las solicitudes.
    				i_packetReady = 0;
    				i_OnOff = 0;
    				i_fifo_empty = 0;
    				//i_fifo_dataOut = 0x0000010E; //270 - 0001 0000 1110
    				i_fifo_dataOut = 0x00000004; //4 - 0000 0000 0100 //n=4 flits a enviara

						for (int i=0; i<2; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}

						//Paquete listo y ya en la entrada i_fifo_dataOut
    				i_packetReady = 1;
    				i_OnOff = 0;
    				i_fifo_empty = 0;

						for (int i=0; i<5; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}
						//Ya está disponible el receptor para recibir el paquete
					i_packetReady = 0;
					i_OnOff = 1;
					i_fifo_empty = 0;

						for (int i=0; i<6; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}
//
			for(int i=0;i<6;i++){
						// Cuando ya no esté disponible, decremento la cuenta en 1
					i_packetReady = 0;
					i_OnOff = 0;
					i_fifo_empty = 0;

						for (int i=0; i<6; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}
						// De nuevo está disponible y hago pop para acceder al siguiente flit de la fifo
					i_packetReady = 0;
					i_OnOff = 1;
					i_fifo_empty = 0;
    				i_fifo_dataOut = 0x00000001 + i; //1 - 0000 0000 0001 //flit 1 de n+2=4+2=6
						for (int i=0; i<4; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}
			}
					i_packetReady = 0;
					i_OnOff = 0;
					i_fifo_empty = 0;

						for (int i=0; i<6; i++){
							i_clock = 0;
							sc_start(10,SC_NS);
							i_clock = 1;
							sc_start(10,SC_NS);
						}


cout << "@" << sc_time_stamp() << "Terminating simulation" << endl;
sc_close_vcd_trace_file(wf);
    return 0;
}
