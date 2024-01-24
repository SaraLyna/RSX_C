package UDP;

import java.net.*;
import java.io.*;
import java.lang.String;

public class ReceiveUDP {
	
	private DatagramSocket socket;
	private int port;
	private byte[] buffer;
	private DatagramPacket p;
	
	public ReceiveUDP(int port) throws IOException {
		this.port = port;
		this.socket = new DatagramSocket(this.port);
		this.buffer = new byte[512];
		this.p = new DatagramPacket(this.buffer,512);
	}
	
	public void receive() throws IOException {
		socket.receive(p);
		System.out.println("paquet reçu de :"+ p.getAddress() +
							"\nport :"         + p.getPort() +
							"\ntaille :" + 	   p.getLength());
		System.out.println("message :" + new String(p.getData()));
		socket.close();
	}
	
	public static void main(String[] args) throws NumberFormatException, IOException {
		ReceiveUDP r = new ReceiveUDP(Integer.parseInt(args[0]));
		r.receive();
	}
}