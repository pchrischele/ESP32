/*===========================================================
  LedModeGui.java - control panel for the ESP32 LED controller

  The buttons send  '1' BLINK, '2' RUN, '3' ALT  to the ESP32
  (115200 baud). The ESP32 answers with "MODE:n" and we show
  that mode on the screen.
  Needs the jSerialComm library.
  (If NetBeans adds a "package ...;" line at the top, keep it.)
-----------------------------------------------------------  */

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.fazecast.jSerialComm.SerialPortMessageListener;
import java.awt.*;
import javax.swing.*;

public class LedModeGui extends JFrame {

    // The number from the ESP32 (0-3) is the position in this list
    String[] names = {"NONE", "BLINK", "RUN", "ALT"};
    Color orange = new Color(230, 126, 34);

    // Things on the screen
    JComboBox<String> portBox = new JComboBox<>();
    JButton refreshBtn = new JButton("Refresh");
    JButton connectBtn = new JButton("Connect");
    JLabel statusLabel = new JLabel("", SwingConstants.CENTER);
    JButton blinkBtn = new JButton("BLINK");
    JButton runBtn = new JButton("RUN");
    JButton altBtn = new JButton("ALT");

    SerialPort port = null;            // null means "not connected"


    public LedModeGui() {
        buildScreen();
        setupButtons();
        listPorts();
        setConnected(false);
    }

    // ---------------------------------------------------------
    // SCREEN
    // ---------------------------------------------------------

    void buildScreen() {
        setTitle("LED Mode Controller");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout(10, 10));
        getRootPane().setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        // top: port list + Refresh + Connect
        JPanel top = new JPanel();
        top.add(new JLabel("Port:"));
        top.add(portBox);
        top.add(refreshBtn);
        top.add(connectBtn);
        add(top, BorderLayout.NORTH);

        // middle: the status text
        statusLabel.setFont(new Font("Arial", Font.BOLD, 40));
        statusLabel.setBorder(BorderFactory.createTitledBorder("Status:"));
        add(statusLabel, BorderLayout.CENTER);

        // bottom: the 3 mode buttons
        JPanel bottom = new JPanel(new GridLayout(1, 3, 10, 10));
        bottom.setPreferredSize(new Dimension(0, 60));
        bottom.add(blinkBtn);
        bottom.add(runBtn);
        bottom.add(altBtn);
        add(bottom, BorderLayout.SOUTH);

        setSize(520, 320);
        setLocationRelativeTo(null);   // open in the middle of the screen
    }

    // What each button does when it is clicked
    void setupButtons() {
        blinkBtn.addActionListener(e -> send('1'));
        runBtn.addActionListener(e -> send('2'));
        altBtn.addActionListener(e -> send('3'));

        refreshBtn.addActionListener(e -> listPorts());

        connectBtn.addActionListener(e -> {
            if (port == null) {
                connect();
            } else {
                disconnect();
            }
        });
    }

    // Turn the buttons on/off and update the texts
    void setConnected(boolean connected) {
        if (connected) {
            connectBtn.setText("Disconnect");
            statusLabel.setText("...");
        } else {
            connectBtn.setText("Connect");
            statusLabel.setText("NOT CONNECTED");
        }
        statusLabel.setForeground(Color.GRAY);

        // port list: only usable while NOT connected
        portBox.setEnabled(!connected);
        refreshBtn.setEnabled(!connected);

        // mode buttons: only usable while connected
        blinkBtn.setEnabled(connected);
        runBtn.setEnabled(connected);
        altBtn.setEnabled(connected);
    }

    // Show the mode (0-3) in the big status text
    void showMode(int n) {
        statusLabel.setText(names[n]);

        if (n == 0) {
            statusLabel.setForeground(Color.GRAY);
        } else {
            statusLabel.setForeground(orange);
        }
    }

    // ---------------------------------------------------------
    // SERIAL PORT
    // ---------------------------------------------------------

    // Fill the drop-down list with the ports of the computer
    void listPorts() {
        portBox.removeAllItems();
        for (SerialPort p : SerialPort.getCommPorts()) {
            portBox.addItem(p.getSystemPortName());
        }
    }

    void connect() {
        String name = (String) portBox.getSelectedItem();
        if (name == null) {
            JOptionPane.showMessageDialog(this, "No port found. Plug in the ESP32 and press Refresh.");
            return;
        }

        SerialPort newPort = SerialPort.getCommPort(name);
        newPort.setComPortParameters(115200, 8, SerialPort.ONE_STOP_BIT, SerialPort.NO_PARITY);
        if (!newPort.openPort()) {
            JOptionPane.showMessageDialog(this, "Cannot open " + name + ".\nClose the Serial Monitor and try again.");
            return;
        }

        // This runs every time a full line (ending with \n) arrives from the ESP32
        newPort.addDataListener(new SerialPortMessageListener() {
            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_RECEIVED; }
            public byte[] getMessageDelimiter() { return new byte[] { '\n' }; }
            public boolean delimiterIndicatesEndOfMessage() { return true; }

            public void serialEvent(SerialPortEvent event) {
                String line = new String(event.getReceivedData()).trim();
                // the screen must be updated from the screen's own thread
                SwingUtilities.invokeLater(() -> readLine(line));
            }
        });

        port = newPort;
        setConnected(true);
        send('?');                     // ask the ESP32 for its current mode
    }

    void disconnect() {
        port.removeDataListener();
        port.closePort();
        port = null;
        setConnected(false);
    }

    // Send one character to the ESP32
    void send(char c) {
        if (port != null) {
            port.writeBytes(new byte[] { (byte) c }, 1);
        }
    }

    // A line from the ESP32, for example "MODE:2"
    void readLine(String line) {
        int i = line.indexOf("MODE:");
        if (i < 0) return;                       // not a MODE line (boot messages) -> ignore
        if (line.length() <= i + 5) return;      // nothing after "MODE:" -> ignore

        int n = line.charAt(i + 5) - '0';        // the digit after "MODE:" as a number
        if (n >= 0 && n <= 3) {
            showMode(n);
        }
    }

    public static void main(String[] args) {
        new LedModeGui().setVisible(true);
    }
}
