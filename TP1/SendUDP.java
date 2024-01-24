package UDP;

import java.net.*;
import java.io.*;

public class SendUDP {

	// The Computer name. For exemple name = a11p23
	private String name;
	private int port; 
	String msg;
		
	public SendUDP(String n, int p, String m) {
		this.name = n;
		this.port = p;
		this.msg = m;
	}
	
	public void send() throws IOException {
		DatagramSocket socket = new DatagramSocket(this.port);
		InetAddress dst = InetAddress.getByName(this.name);
		byte[] buffer = this.msg.getBytes();
		DatagramPacket p = new DatagramPacket(buffer, buffer.length,dst,this.port);
		socket.send(p);
		socket.close();
	}
	
	public static void main(String[] args) throws IOException {
		SendUDP s = new SendUDP(args[0], Integer.parseInt(args[1]), args[2]);
		s.send();
	}

}
