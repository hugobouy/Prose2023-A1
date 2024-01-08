package com.prose.a1.aop.modelTest.employeeManagerTest;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;

import com.prose.a1.aop.model.employeeManager.CacheEmployeeManager;


public class CacheEmployeeManagerTest {

    private CacheEmployeeManager cacheEmployeeManager;

    @Before
    public void setUp() {
        cacheEmployeeManager = new CacheEmployeeManager();
    }

    @After
    public void tearDown() {
        cacheEmployeeManager.deleteCache();
    }

    @Test
    public void testSetNameListEmployee() {
        // Création d'une liste d'employés de test
        cacheEmployeeManager.setDataEmployee(1, "Martin", "Jean", 6, new byte[][]{{0x08, 0x00}, {0x09, 0x00}});

        List<String> expectedList = new ArrayList<>();
        expectedList.add("Martin");

        List<String> nameListEmployee = cacheEmployeeManager.getNameListEmployee();

        assertEquals(expectedList, nameListEmployee);
    }

    @Test
    public void testSetFirstNameListEmployee() {
        // Création d'une liste d'employés de test
        cacheEmployeeManager.setDataEmployee(1, "Martin", "Jean", 6, new byte[][]{{0x08, 0x00}, {0x09, 0x00}});

        List<String> expectedList = new ArrayList<>();
        expectedList.add("Jean");

        List<String> firstNameListEmployee = cacheEmployeeManager.getFirstNameListEmployee();

        assertEquals(expectedList, firstNameListEmployee);
    }

    @Test
    public void testSetRoleListEmployee() {
        // Création d'une liste d'employés de test
        cacheEmployeeManager.setDataEmployee(1, "Martin", "Jean", 6, new byte[][]{{0x08, 0x00}, {0x09, 0x00}});

        List<Integer> expectedList = new ArrayList<>();
        expectedList.add(6);

        List<Integer> roleListEmployee = cacheEmployeeManager.getRoleListEmployee();

        assertEquals(expectedList, roleListEmployee);
    }

    @Test
    public void testSetIdEmployee() {
        // Création d'une liste d'employés de test
        cacheEmployeeManager.setDataEmployee(1, "Martin", "Jean", 6, new byte[][]{{0x08, 0x00}, {0x09, 0x00}});

        List<Integer> expectedList = new ArrayList<>();
        expectedList.add(1);

        List<Integer> idListEmployee = cacheEmployeeManager.getIdEmployee();

        assertEquals(expectedList, idListEmployee);
    }

    @Test
    public void testAskCalendarEmployee() {
        // Création d'une liste d'employés de test
        cacheEmployeeManager.setDataEmployee(1, "Martin", "Jean", 5, new byte[][]{{0x00, 0x00}, {0x02, 0x09}, {0x03, 0x0a}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}});

        int[][] expectedIndex = new int[][]{{0x00, 0x00}, {0x02, 0x09}, {0x03, 0x0a}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}};

        int[][] index = cacheEmployeeManager.askCalendarEmployee("Martin");

        assertArrayEquals(expectedIndex, index);
    }
}
