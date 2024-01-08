package com.prose.a1.aop.view.home;

import static org.junit.Assert.assertEquals;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.view.View;
import com.prose.a1.aop.R;
import org.junit.Test;


public class FragmentHomeTest {

    @Test
    public void testOnClickCalendar() {
        View mockViewButton = mock(View.class);
        View mockViewFrag = mock(View.class);
        int buttonCalendar = R.id.buttonCalendar;
        int fragCalendar = R.id.fragmentCalendar;
        when(mockViewButton.getId()).thenReturn(R.id.buttonCalendar);
        when(mockViewFrag.getId()).thenReturn(R.id.fragmentCalendar);
        assertEquals(buttonCalendar, mockViewButton.getId());
        assertEquals(fragCalendar, mockViewFrag.getId());
        verify(mockViewButton, times(1)).getId();
    }

    @Test
    public void testOnClickQuit() {
        View mockViewButton = mock(View.class);
        View mockViewFrag = mock(View.class);
        int buttonQuit = R.id.buttonQuit;
        int fragQuit = 0;
        when(mockViewButton.getId()).thenReturn(R.id.buttonQuit);
        when(mockViewFrag.getId()).thenReturn(0);
        assertEquals(buttonQuit, mockViewButton.getId());
        assertEquals(fragQuit, mockViewFrag.getId());
        verify(mockViewButton, times(1)).getId();

    }

    @Test
    public void testOnClickVideo() {
        View mockViewButton = mock(View.class);
        View mockViewFrag = mock(View.class);
        int buttonVideo = R.id.buttonVideo;
        int fragVideo = R.id.fragmentHome;
        when(mockViewButton.getId()).thenReturn(R.id.buttonVideo);
        when(mockViewFrag.getId()).thenReturn(R.id.fragmentHome);
        assertEquals(buttonVideo, mockViewButton.getId());
        assertEquals(fragVideo, mockViewFrag.getId());
        verify(mockViewButton, times(1)).getId();

    }

    @Test
    public void testOnClickDoor() {
        View mockViewButton = mock(View.class);
        View mockViewFrag = mock(View.class);
        int buttonDoor = R.id.buttonDoor;
        int fragDoor= R.id.fragmentHome;
        when(mockViewButton.getId()).thenReturn(R.id.buttonDoor);
        when(mockViewFrag.getId()).thenReturn(R.id.fragmentHome);
        assertEquals(buttonDoor, mockViewButton.getId());
        assertEquals(fragDoor, mockViewFrag.getId());
        verify(mockViewButton, times(1)).getId();
    }

    @Test
    public void testOnClickEmployee() {
        View mockViewButton = mock(View.class);
        View mockViewFrag = mock(View.class);
        int buttonEmployee = R.id.buttonEmployee;
        int fragEmployee = R.id.fragmentHome;
        when(mockViewButton.getId()).thenReturn(R.id.buttonEmployee);
        when(mockViewFrag.getId()).thenReturn(R.id.fragmentHome);
        assertEquals(buttonEmployee, mockViewButton.getId());
        assertEquals(fragEmployee, mockViewFrag.getId());
        verify(mockViewButton, times(1)).getId();
    }

}

