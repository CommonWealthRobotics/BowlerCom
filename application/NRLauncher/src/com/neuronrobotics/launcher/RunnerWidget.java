package com.neuronrobotics.launcher;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.border.Border;



import net.miginfocom.swing.MigLayout;


public class RunnerWidget extends JPanel {
	/**
	 * 
	 */
	private static final long serialVersionUID = -7805049193540804573L;
	private ArrayList<JarWidget> jars = new ArrayList<JarWidget> ();
	private File file = null;
	private File log = null;
	private JLabel fileLabel = new JLabel();
	private JButton run = new JButton("Run");
	private JButton stop = new JButton("Stop");
	private String text="";
	private JarRunner runner=null;
	private String launchDir = ".";
	//private JPanel scroller = new  JPanel();
	private JScrollPane scrollPanel;// = new JScrollPane(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
	private ImageIcon logo = new ImageIcon(NRLauncher.class.getResource("image/logo2.png"));
	private JTextArea output = new JTextArea(){
		/**
		 * 
		 */
		private static final long serialVersionUID = 4940315437886755312L;

		@Override
	    protected void paintComponent(Graphics g) {
	        super.paintComponent(g);
	        try {
	            URL url = NRLauncher.class.getResource("image/logo.png");
	            final java.awt.image.BufferedImage image = javax.imageio.ImageIO.read(url);

	            if(getFrame() != null){
		            int y = getHeight()-(getFrame().getHeight() - image.getHeight())/2;
		            int x = (getFrame().getWidth() - image.getWidth())/2;
		            //g.drawImage(image, x, y, this);
	            }
	        } catch(Exception e) {
	        	e.printStackTrace();
	        }
	    }
	};
	private boolean isRunning;
	private String oldtxt;
	private JPanel jarPanel = new JPanel(new MigLayout());
	private JButton refresh = new JButton("Refresh Jars");

	private JFrame frame;
	public RunnerWidget(JFrame f,String dir){
		setFrame(f);
		launchDir = new File(dir).getAbsolutePath();
		setLayout(new MigLayout());
		
		
		refresh.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				refreshJars();
			}
		});
		add(jarPanel,"wrap");
		add(fileLabel,"wrap");
		JPanel control = new JPanel();
		control.add(run);
		control.add(stop);
		//control.add(new JLabel(logo));
		add(control,"wrap");
		output.setLineWrap(true);
		scrollPanel = new JScrollPane(output);
		scrollPanel.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(scrollPanel,"wrap");
		
		
		run.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				setRunning(true);
				runner = new JarRunner();
				runner.start();
				new Thread(){
					public void run(){
						while(isRunning ){
							try {
								Thread.sleep(200);
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							String tmp= getText();
							output.setText(tmp);
							try{
								output.setCaretPosition(tmp.length()-1);
								output.moveCaretPosition(tmp.length()-1);
							}catch(IllegalArgumentException ex){
								ex.printStackTrace();
							}
						}
					}
				}.start();
			}
		});
		stop.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				setRunning(false);
				runner.kill();
			}
		});
		refreshJars();
	}
	public void refreshJars(){
		clearFile();
		jars.clear();
		jarPanel.removeAll();
		JPanel control = new JPanel(new MigLayout());
		control.add(refresh,"wrap");
		
		jars.add(new JarWidget(this, "/usr/local/NeuronRobotics/RDK/bin/nr-console.jar"));
		
		File dir = new File(launchDir);

		String[] children = dir.list();
		if (children == null) {
		    // Either dir does not exist or is not a directory
		} else {
		    for (int i=0; i<children.length; i++) {
		        if(children[i].contains(".jar") && !children[i].contains("nr-console.jar")){
		        	jars.add(new JarWidget(this, launchDir+"/"+children[i]));
		        }

		    }
		}
		
		for (JarWidget jw:jars){
			jw.invalidate();
			jw.repaint();
			jw.setVisible(true);
			control.add(jw,"wrap");
		}
		jarPanel.add(control);
		jarPanel.add(new JLabel(logo));
		setVisible(true);
	}
	public File getFile(){
		return file;
	}
	public void clearFile(){
		fileLabel.setText("Using File: none");
		for (JarWidget jw:jars){
			jw.selsect(false);
		}
		setRunning(false);
		run.setEnabled(false);
		stop.setEnabled(false);
		if(runner != null)
			runner.kill();
		setVisible(true);
	}
	@Override
	public void setVisible(boolean b){
		for (JarWidget jw:jars){
			jw.setVisible(b);
		}
		jarPanel.setVisible(b);
		if(scrollPanel!=null)
			scrollPanel.setVisible(b);
		fileLabel.setVisible(b);
		run.setVisible(b);
		stop.setVisible(b);
		if(getFrame().isVisible())
			getFrame().setVisible(b);
	}
	public void setFile(File file2) {
		run.setEnabled(true);
		file =file2;
		fileLabel.setText("Using File: "+file.getName());
		fileLabel.setVisible(true);
		String date = new SimpleDateFormat("yyyy-MM-dd-HH:mm").format(new Date());

		String logFileName = launchDir+"/"+file.getName().substring(0, file.getName().indexOf(".jar"))+".log."+date+".txt";
		try{
			
			log = new File(logFileName);
			if(!log.exists()){
				System.out.println("Created: "+logFileName);
				log.createNewFile();
			}else{
				System.out.println("Exists: "+logFileName);
			}
	        
    	    text="\n\nStarting new run: "+new SimpleDateFormat("yyyy-MM-dd HH:mm").format(new Date())+"\n\n";
		}catch(Exception ex){
			ex.printStackTrace();
		}
	}
	@Override 
	public void setSize(int width,int height){
		super.setSize(width,height);

		Dimension d = new Dimension(width-30,height - jarPanel.getHeight());
		
		scrollPanel.setPreferredSize(new Dimension(d.width,400));
		scrollPanel.setSize(new Dimension(d.width,300));
	}
	private void setRunning(boolean b){
		isRunning = b;
		if(b){
			run.setEnabled(false);
			stop.setEnabled(true);
		}else{
			run.setEnabled(true);
			stop.setEnabled(false);
		}
	}
	private synchronized void addText(String s){
		//System.out.print(s);
		text+=s;
	}
	private String getText(){
		return text;
	}

	private class JarRunner extends Thread{
		private Process child=null;
		public void kill(){
			if(child!=null){
				try {
					killUnixProcess(child);
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		public void run(){
			try {
				setRunning(true);
			    // Execute command
				String command = "java -jar \'"+getFile().getAbsolutePath()+"\'";

				ProcessBuilder pb = new ProcessBuilder( "java","-jar",getFile().getAbsolutePath() );

				pb.directory( new File(".") );

				pb.redirectErrorStream( true );

				child = pb.start();
				
			    System.out.println("Starting: "+command);
			    
			    InputStream in = child.getInputStream();
			    DataInputStream input = new DataInputStream(in);
	    	    BufferedReader br = new BufferedReader(new InputStreamReader(input));
			    String strLine ;
			    while (true) {
			    	if((strLine = br.readLine()) != null)   {
			    		addText(strLine+"\n");
		    	    }
			    	else
			    		break;
			    }
			    in.close();
			    
			    System.out.println("Done: "+command);
			    //System.out.println(getText());
				if(file==null){
					return;
				}
				try{
		        	oldtxt = "";
		    	    FileInputStream fstream = new FileInputStream(log);
		    	    // Get the object of DataInputStream
		    	    input = new DataInputStream(fstream);
		    	    br = new BufferedReader(new InputStreamReader(input));
		    	    //Read File Line By Line
		    	    while ((strLine = br.readLine()) != null)   {
		    	      // Print the content on the console
		    	    	oldtxt+=strLine+"\n";
		    	    }
		    	    //Close the input stream
		    	    in.close();
	    	    }catch (Exception e){//Catch exception if any
	    	      System.err.println("Error: " + e.getMessage());
	    	    }
				try {
					FileWriter fstream1 = new FileWriter(log);
					BufferedWriter out1 = new BufferedWriter(fstream1);
					out1.write(oldtxt+getText());
				    out1.close();
				} catch (IOException er) {
					er.printStackTrace();
				}
				setRunning(false);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}


	}
	
	public static int getUnixPID(Process process) throws Exception
	{
	    System.out.println(process.getClass().getName());
	    if (process.getClass().getName().equals("java.lang.UNIXProcess"))
	    {
	    	System.out.println("Killing");
	        @SuppressWarnings("rawtypes")
			Class cl = process.getClass();
	        java.lang.reflect.Field field = cl.getDeclaredField("pid");
	        field.setAccessible(true);
	        Object pidObject = field.get(process);
	        return (Integer) pidObject;
	    } else
	    {
	        throw new IllegalArgumentException("Needs to be a UNIXProcess");
	    }
	}

	public static int killUnixProcess(Process process) throws Exception
	{
	    int pid = getUnixPID(process);
	    //System.out.println("calling: "+"kill " + pid);
	    ProcessBuilder pb = new ProcessBuilder( "kill",new Integer(pid).toString());
	    Process p= pb.start();
	    while(p.getInputStream().read()!=-1){
	    	Thread.sleep(10);
	    }
	    return 0;
	}
	public void setLaunchDir(String launchDir) {
		// TODO Auto-generated method stub
		
	}
	public void setFrame(JFrame frame) {
		this.frame = frame;
	}
	public JFrame getFrame() {
		return frame;
	}
}