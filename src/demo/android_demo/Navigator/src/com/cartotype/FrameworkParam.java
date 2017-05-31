/*
FrameworkParam.java
Copyright (C) 2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype;

/**
Parameters for creating a CartoType framework when more detailed control is needed.
For example, file buffer size and the maximum number of buffers can be set.
*/
public class FrameworkParam
    {
    /** The map. Must not be null or empty. */
    public String iMapFileName;
    /** The style sheet. Must not be null or empty. */
    public String iStyleSheetFileName;
    /** The font file. Must not be null or empty. */
    public String iFontFileName;
    /** The width of the map in pixels. Must be greater than zero. */
    public int iViewWidth;
    /** The height of the map in pixels. Must be greater than zero. */
    public int iViewHeight;
    /** If non-null, an encryption key to be used when loading the map. */
    public byte[] iEncryptionKey;
    /** The file buffer size in bytes. If it is zero or less the default value is used. */
    public int iFileBufferSizeInBytes;
    /** The maximum number of file buffers. If it is zero or less the default value is used. */
    public int iMaxFileBufferCount;
    /**
    The number of levels of the text index to load into RAM.
    Use values from 1 to 5 to save RAM but make text searches slower.
    The value 0 causes the default number of levels (6) to be loaded.
    The value -1 disables text index loading.
    */
    public int iTextIndexLevels;
    }
