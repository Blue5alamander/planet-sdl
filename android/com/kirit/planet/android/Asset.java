package com.kirit.planet.android;

import android.content.res.AssetManager;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

public class Asset {
    public static native void useManager(AssetManager assetManager);

    public static byte[] loader(AssetManager am, String fn) {
        try {
            InputStream in = am.open(fn);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            for (int length = in.read(buffer); length != -1; length = in.read(buffer)) {
                stream.write(buffer, 0, length);
            }
            return stream.toByteArray();
        } catch (IOException e ) {
            return null;
        }
    }
}
