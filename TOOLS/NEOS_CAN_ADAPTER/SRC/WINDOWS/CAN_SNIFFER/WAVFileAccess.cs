using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO;

/// <summary>
/// Summary description for Class1
/// </summary>
namespace WAV
{

public class WAVFileAccess
{
	public WAVFileAccess()
	{
		//
		// TODO: Add constructor logic here
		//
	}

    public int RIFFLength;
    public short NumChannels;
    public int SampleRate;
    public short Resolution;
    public int WaveDataLength;

    public bool ReadWAVHeader(string spath)
    {

        try
        {

            FileStream fs = new FileStream(spath, FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fs);
            RIFFLength = (int)fs.Length - 8;
            fs.Position = 22;
            NumChannels = br.ReadInt16();
            fs.Position = 24;
            SampleRate = br.ReadInt32();
            fs.Position = 34;

            Resolution = br.ReadInt16();
           
            fs.Position = 40;
            WaveDataLength = (int)br.ReadInt32();
            br.Close();
            fs.Close();
            return true;
        }
        catch
        {
            return false;
        }

    }


    public byte[] ReadWAVData(string spath)
        {
            

        if (ReadWAVHeader(spath) == true)
            {

            try
                {

                FileStream fs = new FileStream(spath, FileMode.Open, FileAccess.Read);
                BinaryReader br = new BinaryReader(fs);

                fs.Position = 44;

                byte[] WaveData = br.ReadBytes(WaveDataLength);
                br.Close();
                fs.Close();
                return WaveData;
                
                }
            catch
                {
                return null;
                }


            }
        else
            {
            return null;
            }

        

        }

}

}