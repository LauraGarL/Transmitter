#ifndef TX_H_
#define TX_H_

#include <iostream>

using namespace std;
#include "systemc.h"

#define DATA_WIDTH (8)
#define noFlitPacket (4)
//
//// Función constexpr para calcular el logaritmo en base 2 en tiempo de compilación
//constexpr int log2(int n, int p = 0) {
//    return (n <= 1) ? p : log2(n / 2, p + 1);
//}
//
//const int LOG2_N = log2(noFlitPacket+2);

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

	 sc_signal<int> state, next_state;

	 //sc_int<LOG2_N> count;
	 sc_int<9> count;

	void transition(){
		if (i_reset.read()){
			next_state = 0;
			o_Req.write(0);
			o_SoP.write(0);
			o_fifo_pop.write(0);
			cout<<"@"<< sc_time_stamp() <<":: Reseted IDLE "<< endl;
		}else{
			  switch (state.read()) {
			  	  case 0:
			  		  // Si hay un paquete listo (i_packetReady=1), leo los 9-bit lsb del dato en la salida de la FIFO
			  		  //correspondientes al número de flits a transmitir de la cabecera del paquete, y guardo la cantidad en la variable "count".
			      if (i_packetReady.read()) {
			    	  next_state = 1;

			    	  cout<<":: from S0:IDLE to S1:N_READ"<< endl;

			    	  o_Req.write(0);
			    	  o_SoP.write(0);
			    	  o_fifo_pop.write(0);
			    	  //count = i_fifo_dataOut.read().range(8,0);
			    	  count = i_fifo_dataOut.read();

			      } else {
			          next_state = 0;}break;
			      case 1:
			    	  // Ya con el # de flits a enviar, espero la señal de disponibilidad para recibir un flit (i_OnOff = 1) y pongo en 1 a o_Req y o_SoP
			    	  // y aumento en 2 a la variable "count", correspondiente a los flits de la cabecera y la cola del paquete a enviar.
			      if (i_OnOff.read()) {
			    	  next_state = 2;

			    	  cout<<":: from S1:N_READ to S2:START OF TRANSMISSION"<< endl;

			    	  count = count + 2;
			    	  o_Req.write(1);
			    	  o_SoP.write(1);
			    	  o_fifo_pop.write(0);
			      } else {
			          next_state = 1;}break;
			      case 2:
			    	  // Espero por la señal i_OnOff=0 para poner en 0 las señales o_Req y o_SoP y disminuyo la cuenta "count" en 1.
			    	  if (!i_OnOff.read()) {
			    	  	next_state = 3;

			    	  	cout<<":: from S2:START OF TRANSMISSION to S3:TRANSMITTING"<< endl;

			    	  	o_Req.write(0);
			    	  	o_SoP.write(0);
			    	  	o_fifo_pop.write(0);
			    	  	count--;
				      } else {
				    	next_state = 2;}break;
			      default:

			    	  //Cuando la cuenta "count" sea 0, habré terminado de leer los flits del paquete incluyendo la cola(tail).
			    	  if(count == 0){//SUBIR
			    	  	next_state = 0;

			    	  	cout<<":: from S3:TRANSMITTING to S0:IDLE"<< endl;
 			    		 o_Req.write(0);
 			    		 o_SoP.write(0);
 			    		 o_fifo_pop.write(0);
		    	 	 }
			    	  // Si hay disponibilidad para enviar un flit (i_OnOff=1), pongo en 1 o_Req y o_fifo_pop, y pongo en 0  a o_SoP por que
			    	  // ya no es un flit de inicio de paquete.
			    	  else if ( i_OnOff.read() && !(i_fifo_empty.read()) ) {
			    	 	next_state = 2;

			    	 	cout<<":: from S3:TRANSMITTING to S2::CURRENT TRANSMISSION"<< endl;

			    	 	o_Req.write(1);
			    	 	o_SoP.write(0);
			    	 	o_fifo_pop.write(1);

			    	 }
			    	  //Si no hay disponibilidad de recibir un flit (i_OnOff=0) ó la FIFO tiene almenos un elemento(no está vacía).
			    	 else if( !i_OnOff.read() ){
			    	    next_state = 3;}break;
			            	} // end switch
			        }// end else->reset
		}// end transition

	void update() {		//State Update
	    if (i_reset.read()) {
	    	state = 0;
		} else if (i_clock.posedge()) {
		    state = next_state;
		}
					}

//	void outputs() {
//	    switch (state.read()) {
//			case S0:
//				{o_Req.write(0); o_SoP.write(0);} // Req en 0 en S0
//			    break;
//            case S1:
//            	{o_Req.write(0); o_SoP.write(0);} // Req en 0 en S1
//            	break;
//            case S2:
//            	{o_Req.write(1); o_SoP.write(1);} // Req en 0 en S2 (Req = 0 cuando pasa a S1)
//               break;
//            default:
//            	{o_Req.write(0); o_SoP.write(0);} // Valor por defecto
//             break;
//			        }
//			    }

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
			        sensitive << i_clock.pos() << i_reset << i_packetReady << i_OnOff << i_fifo_empty;

			        SC_METHOD(update);
			        sensitive << i_clock.pos() << i_reset;

//			        SC_METHOD(outputs);
//			        sensitive << state;
			    }
			};
#endif /* TX_H_ */
