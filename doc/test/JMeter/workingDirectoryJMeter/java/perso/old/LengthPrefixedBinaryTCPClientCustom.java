/**
 * TCP Sampler Client qui envoie des messages binaires préfixés par la longueur du message en considérant la taille du préfixe
 * dans le calcul de la longueur (contrairement à LengthPrefixedBinaryTCPClientImpl.java qui soustrait la taille du préfixe).
 * 
 * @date 06/06/32
 * @author Eliot Coulon (eliot.coulon@reseau.eseo.fr), modifié par Hugo Bouy
 */

package perso;

import java.io.InterruptedIOException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.apache.jmeter.samplers.SampleResult;
import org.apache.jmeter.util.JMeterUtils;
import org.apache.jorphan.util.JOrphanUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import org.apache.jmeter.protocol.tcp.sampler.*;

public class LengthPrefixedBinaryTCPClientCustom extends TCPClientDecorator {
    private static final Logger log = LoggerFactory.getLogger(LengthPrefixedBinaryTCPClientImpl.class);

    private final int lengthPrefixLen = JMeterUtils.getPropDefault("tcp.binarylength.prefix.length", 2); // $NON-NLS-1$

    public LengthPrefixedBinaryTCPClientCustom() {
        super(new BinaryTCPClientImpl());
        tcpClient.setEolByte(Byte.MAX_VALUE+1);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void write(OutputStream os, String s) throws IOException {
        byte[] data = hexStringToByteArray(s); // Convert hex-encoded string to byte array
        byte[] lengthPrefix = intToByteArray(data.length + lengthPrefixLen, lengthPrefixLen); // Calculate the length prefix
        os.write(lengthPrefix); // Write the length prefix
        os.write(data); // Write the data
        os.flush(); // Flush the output stream
        if (log.isDebugEnabled()) {
            log.debug("Wrote: " + s.length() / 2 + " bytes");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void write(OutputStream os, InputStream is) throws IOException {
        this.tcpClient.write(os, is);
    }

    @Deprecated
    public String read(InputStream is) throws ReadException {
        log.warn("Deprecated method, use read(is, sampleResult) instead");
        return read(is, new SampleResult());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String read(InputStream is, SampleResult sampleResult) throws ReadException {
        byte[] lengthBuffer = new byte[lengthPrefixLen];
        byte[] msg = null;

        try {
            // Read the length prefix
            int bytesRead = JOrphanUtils.read(is, lengthBuffer, 0, lengthPrefixLen);
            if (bytesRead == lengthPrefixLen) {
                int msgLen = byteArrayToInt(lengthBuffer) - lengthPrefixLen;
                msg = new byte[msgLen];
                int bytes = JOrphanUtils.read(is, msg, 0, msgLen);
                if (bytes < msgLen) {
                    log.warn("Incomplete message read, expected: {} got: {}", msgLen, bytes);
                }
            }

            String buffer = JOrphanUtils.baToHexString(msg);
            if (log.isDebugEnabled()) {
                log.debug("Read: " + (msg != null ? msg.length + lengthPrefixLen : 0) + "\n" + buffer);
            }
            return buffer;
        }
        catch(InterruptedIOException ex) {
            log.info("Read timeout (no response expected)");
            return "";
        }
        catch (IOException e) {
            throw new ReadException("", e, JOrphanUtils.baToHexString(msg));
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public byte getEolByte() {
        return tcpClient.getEolByte();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setEolByte(int eolInt) {
        throw new UnsupportedOperationException("Cannot set eomByte for prefixed messages");
    }

    private byte[] hexStringToByteArray(String hexString) {
        hexString = hexString.replaceAll("\\s+", ""); // Supprimer les espaces de la chaîne hexadécimale
        int len = hexString.length();
        byte[] byteArray = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            byteArray[i / 2] = (byte) ((Character.digit(hexString.charAt(i), 16) << 4)
                    + Character.digit(hexString.charAt(i + 1), 16));
        }
        return byteArray;
    }
}
