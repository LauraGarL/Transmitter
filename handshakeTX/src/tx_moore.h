#ifndef TX_H_
#define TX_H_
// TODO LIST
//paquetes seguidos
//paquetes seguido y tiempo muerto
// tiempo entre flits
//

#include <iostream>

using namespace std;
#include "systemc.h"

#define DATA_WIDTH (8)
#define noFlitPacket (4)

SC_MODULE(TX){
	sc_in_clk 						i_clock;		//Input: System Clock
	sc_in<bool>						i_reset;		//Input: System Reset
	sc_in<bool>						i_packetReady;	//Input: Indicates that there is a pack to send
	sc_in<bool>						i_OnOff;		//Input: Indicates availability to receive a flit
	sc_in<bool>						i_fifo_empty;	//Input: Indicates that the FIFO is empty
	sc_in<sc_uint<DATA_WIDTH>>		i_fifo_dataOut;	//Input: Data_out from FIFO
	sc_out<bool> 					o_Req;			//Output: Indicates whether the request was granted
	sc_out<bool> 					o_SoP;			//Output: Indicates the start of a packet
	sc_out<bool> 					o_fifo_pop;		//Output: Remove an element from  FIFO

	 sc_signal<int> state, prev_state;

	 sc_int<9> count;

//	 sc_signal<bool> prev_OnOff;  // Señal para almacenar el valor anterior de i_OnOff

	void transition(){
		if( i_reset.read() && i_clock.posedge()  ){
			state = 0;
			o_Req.write(0);
			o_SoP.write(0);
			o_fifo_pop.write(0);
//			prev_OnOff.write(0); //
			count = 0;
			prev_state = 0;
			cout<<"@"<< sc_time_stamp() <<":: Reseted IDLE "<< endl;
		}else if(i_clock.posedge())// Aseguro que ocurra flanco positivo del reloj
			{

			prev_state = state;

			  switch (state.read()) {
			  	  case 0:
						o_Req.write(0);
						o_SoP.write(0);
						o_fifo_pop.write(0);
						count = 0;

						if (i_packetReady.read()) {
							state = 1;
							cout<<":: from S0:IDLE to S1:N_READ"<< endl;
						} else { state = 0;}break;

			  	  case 1:
			    	  o_Req.write(0);
			    	  o_SoP.write(0);
			    	  o_fifo_pop.write(0);
			    	  count = i_fifo_dataOut.read() +2;//Agrego los dos flits restantes correspondientes a la cabecera y a la cola.

			    	  if (i_OnOff.read()) {
			    		  state = 2;
			    		  cout<<":: from S1:N_READ to S2:START OF TRANSMISSION"<< endl;
			    	  } else { state = 1;}break;

			      case 2:
			    	  o_Req.write(1);
			    	  o_SoP.write(1);
			    	  o_fifo_pop.write(0);

			    	  if (!i_OnOff.read()) {
			    		  state = 3;
			    		  cout<<":: from S2:START OF TRANSMISSION to S3:TRANSMITTING"<< endl;
				      } else { state = 2;}break;

			      case 3:
			    	  o_Req.write(0);
			    	  o_SoP.write(0);
			    	  o_fifo_pop.write(0);

	                  if (prev_state != 3) {
	                      count--;
	                      cout << ":: Count decremented in S3, from previous state: " << prev_state.read() << endl;
	                  }

			    	  //Cuando la cuenta "count" sea 0, habré terminado de leer los flits del paquete incluyendo la cola(tail).
			    	  if(count == 0){ state = 0;
			    	  	  cout<<":: from S3:TRANSMITTING to S0:IDLE"<< endl;
			    	  }
			    	  else if (i_OnOff.read() && !(i_fifo_empty.read()) ) {
			    	  	  	 	state = 4;
			    	  	  	 	cout<<":: from S3:TRANSMITTING to S2::RECEIVED"<< endl;

			    	  }else { state = 3; }break;

// Condición que busca un cambio de flanco positivo para restar sólo una vez.
//			    	  else if (!prev_OnOff.read() && i_OnOff.read() && !(i_fifo_empty.read()) ) {
//			    	 	state = 4;
//			    	 	cout<<":: from S3:TRANSMITTING to S2::RECEIVED"<< endl;
//			    	  	count--;
//			    	  	prev_OnOff.write(i_OnOff.read());
//			    	 }

			    	  //Si no hay disponibilidad de recibir un flit (i_OnOff=0) ó la FIFO tiene almenos un elemento(no está vacía).

			      default:
			    	 	o_Req.write(1);
			    	 	o_SoP.write(0);
			    	 	o_fifo_pop.write(1);

				    	  if (!i_OnOff.read()) {
				    	  	state = 3;

				    	  	cout<<":: from S2:RECEIVED to S3:TRANSMITTING"<< endl;

					      } else { state = 4;}break;
			  }// end switch
//			  prev_OnOff.write(i_OnOff.read());
			}// end else->reset
	}// end transition

			    SC_CTOR(TX): //	Labeling ports
			    	i_clock("i_clock"),					//Input: System Clock
			    	i_reset("i_reset"),					//Input: System Reset
			    	i_packetReady("i_packetReady"),		//Input: Indicates that there is a pack to send
			    	i_OnOff("i_OnOff"),					//Input: Indicates availability to receive a flit
					i_fifo_empty("i_fifo_empty"),		//Input: Indicates that the FIFO is empty
					i_fifo_dataOut("i_fifo_dataOut"),	//Input: Data_out from FIFO
			    	o_Req("o_Req"),						//Output:  Indicates whether the request was granted
			    	o_SoP("o_SoP"),						//Output:  Indicates the start of a packet
			    	o_fifo_pop("o_fifo_pop")			//Output: Remove an element from  FIFO
			    {
			        SC_METHOD(transition);
			        sensitive << i_clock.pos() << i_reset; // << state <<i_reset << i_packetReady << i_OnOff << i_fifo_empty;
			    }
			};
#endif /* TX_H_ */
