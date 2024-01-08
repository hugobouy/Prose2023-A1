package com.prose.a1.aop.modelTest.employeeManagerTest;

import static org.junit.Assert.assertEquals;

import com.prose.a1.aop.model.employeeManager.Employee;

import org.junit.Test;

public class EmployeeTest {

    @Test
    public void testGetName() {
        String expectedName = "Martin";
        Employee employee = new Employee(1, expectedName, "Jean", 2, new byte[7][2]);
        String actualName = employee.getName();
        assertEquals(expectedName, actualName);
    }

    @Test
    public void testGetFirstName() {
        String expectedFirstName = "Jean";
        Employee employee = new Employee(1, "Martin", expectedFirstName, 2, new byte[7][2]);
        String actualFirstName = employee.getFirstName();
        assertEquals(expectedFirstName, actualFirstName);
    }

    @Test
    public void testGetRole() {
        int expectedRole = 2;
        Employee employee = new Employee(1, "Martin", "Jean", expectedRole, new byte[7][2]);
        int actualRole = employee.getRole();
        assertEquals(expectedRole, actualRole);
    }

    @Test
    public void testGetEmployeeID() {
        int expectedEmployeeID = 1;
        Employee employee = new Employee(expectedEmployeeID, "Martin", "Jean", 2, new byte[7][2]);
        int actualEmployeeID = employee.getEmployeeID();
        assertEquals(expectedEmployeeID, actualEmployeeID);
    }

    @Test
    public void testGetStart() {
        byte expectedStart = 8;
        byte[][] workingHours = {
                { 8, 17 }, { 9, 18 }, { 10, 19 },
                { 8, 16 }, { 9, 17 }, { 10, 18 },
                { 0, 0 } // Sunday
        };
        Employee employee = new Employee(1, "Martin", "Jean", 2, workingHours);
        byte actualStart = employee.getStart(0);
        assertEquals(expectedStart, actualStart);
    }

    @Test
    public void testGetEnd() {
        byte expectedEnd = 17;
        byte[][] workingHours = {
                { 8, 17 }, { 9, 18 }, { 10, 19 },
                { 8, 16 }, { 9, 17 }, { 10, 18 },
                { 0, 0 } // Sunday
        };
        Employee employee = new Employee(1, "Martin", "Jean", 2, workingHours);
        byte actualEnd = employee.getEnd(0);
        assertEquals(expectedEnd, actualEnd);
    }
}
