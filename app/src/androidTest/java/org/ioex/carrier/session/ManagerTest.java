package org.ioex.carrier.session;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.util.Log;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import org.ioex.carrier.common.Synchronizer;
import org.ioex.carrier.common.TestOptions;
import org.ioex.carrier.AbstractCarrierHandler;
import org.ioex.carrier.Carrier;
import org.ioex.carrier.exceptions.IOEXException;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class ManagerTest {
    private static String TAG = "SessionManagerTest";
    private static Carrier carrierInst;

    private static Context getAppContext() {
        return InstrumentationRegistry.getTargetContext();
    }

    private static String getAppPath() {
        return getAppContext().getFilesDir().getAbsolutePath();
    }

    static class TestHandler extends AbstractCarrierHandler {
        Synchronizer synch = new Synchronizer();

        public void onReady(Carrier carrier) {
            synch.wakeup();
        }
    }

    @BeforeClass
    public static void setUp() {
        TestHandler handler = new TestHandler();

        try {
            Carrier.initializeInstance(new TestOptions(getAppPath()), handler);
            carrierInst = Carrier.getInstance();
            carrierInst.start(1000);
            handler.synch.await();

        } catch (IOEXException e) {
            e.printStackTrace();
        }
    }

    @AfterClass
    public static void tearDown() {
        try {
            carrierInst.kill();
        }catch(Exception e) {
            e.printStackTrace();
        }
    }

    @Test
    public void getInstanceWithoutRequestHandler() {
        try {
            Manager sessionMgr = Manager.getInstance(carrierInst);
            assertNotNull(sessionMgr);
            sessionMgr.cleanup();
        } catch (IOEXException e) {
            e.printStackTrace();
            assertTrue(false);
        }
    }

    @Test
    public void getInstanceWithRequestHandler() {
        try {
            Manager sessionMgr = Manager.getInstance(carrierInst, new ManagerHandler() {
                @Override
                public void onSessionRequest(Carrier carrier, String from, String sdp) {
                    Log.i(TAG, "onSessionRequest");
                }
            });
            assertNotNull(sessionMgr);
            sessionMgr.cleanup();
        } catch (IOEXException e) {
            e.printStackTrace();
            assertTrue(false);
        }
    }
}
