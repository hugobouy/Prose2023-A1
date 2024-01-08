package com.prose.a1.aop.communicationTest;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;

import com.prose.a1.aop.communication.Protocol;

import org.junit.Assert;
import org.junit.Test;

import java.nio.ByteBuffer;
import java.util.Arrays;


public class ProtocolTest {



    @Test
    public void testEncodeMessage()
    {
        Protocol protocol = new Protocol();

        // Test avec une trame sans données
        int cmdId = protocol.CMD_ID_ASK_EMPLOYEE_LIST;
        int nbArgs = protocol.NB_ARG_ASK_EMPLOYEE_LIST;
        byte[] expected = {(byte) 0x00, 0x04, 0x0a, 0x00};
        byte[] result = protocol.encodeMessage(cmdId, nbArgs, null);
        assertArrayEquals(new String[]{Arrays.toString(expected)}, new String[]{Arrays.toString(result)});
        assertArrayEquals(expected,result);


        // Test avec une trame avec des données (exemple de subscribeToVideoStream)
        cmdId = protocol.CMD_ID_STREAM;
        nbArgs = protocol.NB_ARG_STREAM;
        byte[] data = {0x01};
        expected = new byte[]{0x00,0x07, 0x01, 0x01,0x00,0x01, 0x01};
        result = protocol.encodeMessage(cmdId, nbArgs, data);
        assertArrayEquals(new String[]{Arrays.toString(expected)}, new String[]{Arrays.toString(result)});

        assertArrayEquals(expected,result);

        // Test avec une trame avec des données (exemple de addEmployee)
        cmdId = protocol.CMD_ID_ADD_EMPLOYEE;
        nbArgs = protocol.NB_ARG_ADD_EMPLOYEE;
        String name = "John";
        String firstName = "Doe";
        byte picture = 0x12;
        int role = 1;
        byte[] workingHours = {0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22};

        String workingHoursString = new String(Arrays.copyOfRange(workingHours, 0, workingHours.length));
        data = String.format("%s|%s|%d|%d|%s", name, firstName, picture, role, workingHoursString).getBytes();

        //expected = taille {0x00,0x26} + CMD_ID_ADD_EMPLOYEE {0x08} + NB_ARG_ADD_EMPLOYEE {0x00, 0x04} + data { 0x4A, 0x6F, 0x68,...}
        expected = new byte[] {0x00,0x26, 0x08, 0x05, 0x00, 0x04, 0x4A, 0x6F, 0x68, 0x6E, 0x00, 0x03, 0x44, 0x6F, 0x65, 0x00, 0x02, 0x31, 0x38, 0x00, 0x01, 0x31, 0x00, 0x0E, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22};

        result = protocol.encodeMessage(cmdId, nbArgs, data);

        assertArrayEquals(new String[]{Arrays.toString(expected)}, new String[]{Arrays.toString(result)});

        assertArrayEquals(expected,result);

    }


    @Test
    public void testDecodeMessage() {
        Protocol protocol = new Protocol();


        //Tests pour la commande UpdateDoorState
        byte[] frameSize = { 0x00, 0x03 };
        byte[] messageDecode = {
                0x07, // cmdId
                0x01, // nbArgs
                0x00, // Valeur de l'état de la porte

        };

        byte[][] expected = {
                { 0x07 }, // cmdId
                { 0x01 }, // nbArgs
                { 0x00 }, // Valeur de l'état de la porte
                null // Fin de message
        };
        byte[][] result = protocol.decodeMessage(frameSize, messageDecode);
        assertArrayEquals(expected, result);
    }
}