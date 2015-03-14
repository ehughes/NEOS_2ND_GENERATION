using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using WAV;
using System.Threading;

namespace npack
{
    class Program
    {
        static void Main(string[] args)
        {

            if (args.Length != 1)
            {
                Console.WriteLine("Usage--> npack profilename");
                return;
            }

            
           
            ProfilePackager MyProfilePackager = new ProfilePackager();
            Console.WriteLine("");
            Console.WriteLine("");
            Console.WriteLine("NEOS Sound Packager V0.2");
            Console.WriteLine("--------------------------------------------");
            Console.WriteLine("Checking Syntax of "+args[0]+".......");
        
            //Check Syntax and extract the information from the file

            MyProfilePackager.BasePath = Path.GetDirectoryName(Path.GetFullPath(args[0])) + "\\";
        
                if (MyProfilePackager.ProcessFile(args[0]) == false)
                {
                    Console.WriteLine(MyProfilePackager.ErrorOutput);
                    Console.WriteLine("Assembly halted on Syntax errors.");
                }
                else
                {
                    Console.WriteLine("Syntax is OK.");
                    if (MyProfilePackager.SoundAssembler() == false)
                    {
                        Console.WriteLine(MyProfilePackager.ErrorOutput);
                        Console.WriteLine("Sound profile assembly halted on errors.");
                    }
                    else
                    {

                        Console.WriteLine("Sound Assembler completed successfully!");
                        Console.WriteLine("Linking.....");

                        if (MyProfilePackager.SoundLinker() == false)
                        {
                            Console.WriteLine(MyProfilePackager.ErrorOutput);
                            Console.WriteLine("Sound profile linking halted on errors.");
                        }

                        else
                        {

                            Console.WriteLine("All steps complete without error.");

                            Console.WriteLine(MyProfilePackager.NumSounds + " sounds packed with a combined size of " + MyProfilePackager.PackedSoundSize + " bytes.");
                            
                            Console.WriteLine("Sound index table written to " + MyProfilePackager.BasePath + "bin\\SoundIndexTable.bin");
                            Console.WriteLine("Raw sound data written to " + MyProfilePackager.BasePath + "bin\\SoundIndexTable.bin");
                            Console.WriteLine("Complete flash image written to " + MyProfilePackager.BasePath + "bin\\bin\\FlashImage.bin");

                        }
                    }

                }

                //Write the whatisaw file
                try
                {
                    StreamWriter OutputLog = new StreamWriter(MyProfilePackager.BasePath + "whatisaw", false);

                    OutputLog.Write(MyProfilePackager.ProcessorOutput);
                    OutputLog.Close();

                }
                catch
                {
                    Console.WriteLine("Error writing to the whatisaw file.");
                }

        }
    }

    public class ProfilePackager
    {
        const int MAX_SOUND_TABLE_ENTRIES = 512;
        const int SOUND_DATA_START_ADDRESS = 0x10000;
        const int SOUND_INDEX_TABLE_START_ADDRESS = 0x0000000;
        const int SAMPLE_RATE = 8000;
        const int PLAY_TIMEOUT_RESOLUTION = 100;

        private StreamWriter sw;
        private int CurrentLine;
        public string ProcessorOutput="";
        public string ErrorOutput="";

        public string BasePath ="";

        ArrayList MySoundRecords = new ArrayList();
        ArrayList MyMacros = new ArrayList();
        bool MultiLineCommentStart = false;
        FileStream fs;
        BinaryWriter bw;

        public UInt32 PackedSoundSize = 0;
        public UInt32 NumSounds = 0;

        public bool CleanOutputDirectories()
        {
            try
            {
                Console.WriteLine("Cleaning bin directory");
                if (Directory.Exists(BasePath + "bin") == true)
                {

                    if (File.Exists(BasePath + "bin\\SoundIndexTable.bin"))
                        File.Delete(BasePath + "bin\\SoundIndexTable.bin");

                    if (File.Exists(BasePath + "bin\\FlashImage.bin"))
                        File.Delete(BasePath + "bin\\FlashImage.bin");


                    if (File.Exists(BasePath + "bin\\RawSoundData.bin"))
                        File.Delete(BasePath + "bin\\RawSoundData.bin");
                }

            }
            catch (Exception Ex)
            {
                ErrorOutput += "Could not clean bin directory. " + Ex.Message + "\r\n";
                return false;
            }

            try
            {
                Console.WriteLine("Cleaning include directory");
                if (Directory.Exists(BasePath + "include") == true)
                {

                    if (File.Exists(BasePath + "include\\SoundMapping.h"))
                        File.Delete(BasePath + "include\\SoundMapping.h");

                }

            }
            catch (Exception Ex)
            {
                ErrorOutput += "Could not clean include directory. " + Ex.Message + "\r\n";
                return false;
            }

            return true;
        }

        public bool SoundLinker()
        {
           
            if (Directory.Exists(BasePath + "bin") == false)
            {
                ErrorOutput += "Could not find 'bin' directory for linking step";
                return false;
            }

            if (File.Exists(BasePath + "bin\\SoundIndexTable.bin") == false)
            {
                ErrorOutput += "Could not find SoundTableIndex.bin file for linking step.";
                return false;
            }

            if (File.Exists(BasePath + "bin\\RawSoundData.bin") == false)
            {
                ErrorOutput += "Could not find RawSoundData.bin file for linking step.";
                return false;
            }




            try
            {
                fs = new FileStream(BasePath + "bin\\FlashImage.bin", FileMode.OpenOrCreate, FileAccess.Write);
                bw = new BinaryWriter(fs);
            }
            catch
            {
                ErrorOutput += "Could open FlashImage.bin for writing.";
                return false;
            }
            

            byte[] SoundIndexTable = File.ReadAllBytes(BasePath + "bin\\SoundIndexTable.bin");

            byte [] Padding = new byte[65536 - SoundIndexTable.Length];

            for(int i=0;i<Padding.Length;i++)
            {
                Padding[i] = 0;
            }

           bw.Write(SoundIndexTable);
           bw.Write(Padding);

           bw.Write(File.ReadAllBytes(BasePath + "bin\\RawSoundData.bin"));
           bw.Close();
           fs.Close();

            return true;
        }
        public bool SoundAssembler()
        {
            //first, go through the list and replace macros 
            bool AssemblyOK = true;

            WAVFileAccess MyWavFileAccess = new WAVFileAccess();
            SoundRecord [] SoundRecords = new SoundRecord[MySoundRecords.Count];
            Macro [] Macros = new Macro[MyMacros.Count];


            //clean the output directories
            if (CleanOutputDirectories() == false)
            {
                AssemblyOK = false;
            }
            
            MySoundRecords.CopyTo(SoundRecords);
                   
           //First, translate the Sound number strings into real ints

            Console.WriteLine("Checking .wav files");
            for(int i=0;i<MySoundRecords.Count;i++)
            {
                bool ValOk = true;
                try
                {
                    SoundRecords[i].TableIndex = Convert.ToInt32(SoundRecords[i].Number);
                }
                catch(Exception Ex)
                {
                    AssemblyOK = false;
                    ValOk = false;
                    ErrorOutput += "Illegal Sound Number specified on Line[" + SoundRecords[i].CodeLine + "].  " + Ex.Message +"\r\n";

                }

                finally
                {
                    if (ValOk == true)
                    {
                        if (SoundRecords[i].TableIndex > MAX_SOUND_TABLE_ENTRIES-1)
                        {
                            ErrorOutput += "Illegal Sound Number specified on Line[" + SoundRecords[i].CodeLine + "].  Index must be between 0 and " +(MAX_SOUND_TABLE_ENTRIES-1) + ".  You specified " + SoundRecords[i].TableIndex + "\r\n";
                            AssemblyOK = false;
                        }
                    }
               }
            }

            if (AssemblyOK == false)
            {
                return false;
            }
            
            //now check for duplicates

            for (int i = 0; i < SoundRecords.Length; i++)
            {
                for (int j = 0; j < SoundRecords.Length; j++)
                {
                    if (i != j)
                    {
                        if (SoundRecords[i].TableIndex == SoundRecords[j].TableIndex)
                        {

                            AssemblyOK = false;
                            ErrorOutput += "Line [" + SoundRecords[i].CodeLine + "] and Line [" + SoundRecords[j].CodeLine + "] specify the same table index. \r\n";

                           
                        }
                    }
                }
            }

            if (AssemblyOK == false)
            {
                Console.WriteLine("Halting on duplicate sound index error.\r\n");
                return false;           
            }
       //     

            for (int i = 0; i < SoundRecords.Length; i++)
            {
                if (File.Exists(BasePath+SoundRecords[i].FileName) == false)
                {
                    ErrorOutput += SoundRecords[i].FileName.Trim() + "  could not be found.  Check path.\r\n";
                    AssemblyOK = false;
                }
            }

            if (AssemblyOK == false)
            {
                return false;
            }
            //Check the wave file formats
            for (int i = 0; i < SoundRecords.Length; i++)
            {
                if (MyWavFileAccess.ReadWAVHeader(BasePath+SoundRecords[i].FileName) == false)
                {
                    AssemblyOK = false;
                    ErrorOutput += "Error opening " + BasePath + SoundRecords[i].FileName + " for wav header access.\r\n";
               
                }
                else
                {
                    if (MyWavFileAccess.Resolution != 8)
                    {

                        AssemblyOK = false;

                        ErrorOutput += SoundRecords[i].FileName.Trim() + " is not 8 bit resolution.  " + MyWavFileAccess.Resolution + " bit resolution specified.\r\n";
                    }

                    if (MyWavFileAccess.SampleRate != SAMPLE_RATE)
                    {
                        AssemblyOK = false;

                        ErrorOutput += SoundRecords[i].FileName.Trim() + " is not sampled at " + SAMPLE_RATE + "Hz. " + MyWavFileAccess.SampleRate + "Hz  specified.\r\n";
             
                    }
                    if (MyWavFileAccess.NumChannels != 1)
                    {
                        AssemblyOK = false;

                        ErrorOutput += SoundRecords[i].FileName.Trim() + " is not single channel. " + MyWavFileAccess.NumChannels + " channels specified.\r\n";
                    }

                    SoundRecords[i].WavDataLength = (UInt32)MyWavFileAccess.WaveDataLength;

                }
            }


            Console.WriteLine(".wav files are OK.....  Assembling sound index table.");

            if (AssemblyOK == false)
            {
             
                return false;
            }


            //Figure Out Padding for Each Sound;

           for(int i=0;i<SoundRecords.Length;i++)
           {
               UInt32 Padding;
               Padding = SoundRecords[i].WavDataLength & 0xFF;
               if (Padding == 0)
               {
                   //If sound aldreay ends on a FLASH page boundary, Just add 2 extra for PAd
                   SoundRecords[i].EndWavPadding = 256;
                    SoundRecords[i].FrontWavPadding = 0;


               }
               else
               {
                   //if not , make the padding one whole page plus the extra
                   SoundRecords[i].EndWavPadding = 256 + (256 - Padding);
                   SoundRecords[i].FrontWavPadding = 0;
                    
               }
                
           }


            //Now, write the sound index table
            try
            {

                if (Directory.Exists(BasePath + "bin") == false)
                {
                    Directory.CreateDirectory(BasePath + "bin");
                }
                          

                fs = new FileStream(BasePath + "bin\\SoundIndexTable.bin", FileMode.OpenOrCreate, FileAccess.Write);
                bw = new BinaryWriter(fs);

            }
            catch
            {
                ErrorOutput += "Could not open SoundIndexTable.bin for writing!.\r\n";
                AssemblyOK = false;
            }
            
            if (AssemblyOK == false)
            {

                return false;
            }

            UInt32 SoundAddressCounter = SOUND_DATA_START_ADDRESS;
            bool SoundRecordMatch = false;
            
            for (int i = 0; i < MAX_SOUND_TABLE_ENTRIES; i++)
            {
                SoundRecordMatch = false;

                for (int j = 0; j < SoundRecords.Length; j++)
                {
                    if (SoundRecords[j].TableIndex == i)
                    {
                        UInt32 TempLength = SoundRecords[j].WavDataLength + SoundRecords[j].FrontWavPadding + SoundRecords[j].EndWavPadding;
                        bw.Write(SoundAddressCounter);
                        bw.Write(TempLength);
                        SoundAddressCounter += TempLength;
                        SoundRecordMatch = true;
                        break;
                    }
                   
                }
                //If none of our sound records match the current index write zeros to the
                //table
                if (SoundRecordMatch == false)
                {
                    bw.Write((UInt32)0);
                    bw.Write((UInt32)0);
                        
                }
            }

            fs.Close();
            bw.Close();

            //Write out the raw data to a file.

            try
            {

                if (Directory.Exists(BasePath + "bin") == false)
                {
                    Directory.CreateDirectory(BasePath + "bin");
                }


            

                fs = new FileStream(BasePath + "bin\\RawSoundData.bin", FileMode.OpenOrCreate, FileAccess.Write);
                bw = new BinaryWriter(fs);

            }
            catch
            {
                ErrorOutput += "Could not open RawSoundData.bin for writing!.\r\n";
                AssemblyOK = false;
            }


            Console.WriteLine("Sound index created.");

            if (AssemblyOK == false)
            {
                return false;
            }


            PackedSoundSize = 0;
            NumSounds = (UInt32)SoundRecords.Length;

            //raw data has to be written in the same order as the Index table was populated

            for (int k = 0; k < MAX_SOUND_TABLE_ENTRIES; k++)
            {
                SoundRecordMatch = false;


                for (int i = 0; i < SoundRecords.Length; i++)
                {

                    //If we have a match, write the data out
                    if (SoundRecords[i].TableIndex == k)
                    {

                        byte[] WavData;

                        WavData = MyWavFileAccess.ReadWAVData(BasePath + SoundRecords[i].FileName);

                        //all around expects 2's complement 8-bit data
                        for (int j = 0; j < WavData.Length; j++)
                        {
                            byte Temp = WavData[j];


                            if (Temp > 0x7f)
                            {
                                WavData[j] = (byte)(Temp - 0x80);
                            }
                            else
                            {
                                WavData[j] |= 0x80;
                            }

                        }



                        if (WavData == null)
                        {
                            AssemblyOK = false;
                            ErrorOutput += "Error opening " + BasePath + SoundRecords[i].FileName + " to extract wave data.\r\n";
                            break;
                        }
                        else
                        {
                            for (int l = 0; l < SoundRecords[i].FrontWavPadding; l++)
                            {
                                bw.Write((byte)0x00);
                            }

                            bw.Write(WavData);

                            for (int l = 0; l < SoundRecords[i].EndWavPadding; l++)
                            {
                                bw.Write((byte)0x00);
                            }

                        }

                        PackedSoundSize += (UInt32)WavData.Length + (UInt32)SoundRecords[i].FrontWavPadding + (UInt32)SoundRecords[i].EndWavPadding;

                    }
                   
                }
            }

            bw.Close();
            fs.Close();

            if (AssemblyOK == false)
            {
                return false;
            }
            
            Console.WriteLine("Raw sound data file created.");




            Console.WriteLine("Writing Headers.....");


   

            //See if the output directory exsists and open
            try
            {

                if (Directory.Exists(BasePath + "include") == false)
                {
                    Directory.CreateDirectory(BasePath + "include");
                }

                sw = new StreamWriter(BasePath + "include\\SoundMapping.h");
              
            }
            catch
            {
                ErrorOutput += "Could not open SoundMapping.h for writing!.\r\n";
                AssemblyOK = false;
            }


        
            if (AssemblyOK == false)
            {
                return false;
            }

            try
            {


                sw.Write("#ifndef _SOUND_MAPPING_H\r\n");
                sw.Write("#define _SOUND_MAPPING_H\r\n");

                sw.WriteLine();
                sw.WriteLine();

                for (int k = 0; k < MAX_SOUND_TABLE_ENTRIES; k++)
                {
            

                    for (int i = 0; i < SoundRecords.Length; i++)
                    {

                        //If we have a match, write the data out
                        if (SoundRecords[i].TableIndex == k)
                        {
                            string NameCleanUp = SoundRecords[i].FileName.Replace(".", "_");
                            UInt32 SoundLen;

                            

                            NameCleanUp =  NameCleanUp.Trim();
                            NameCleanUp = NameCleanUp.ToUpper();
                            NameCleanUp = NameCleanUp.Replace(" ", "_");
                            NameCleanUp = NameCleanUp.Replace("/", "+");
                            NameCleanUp = NameCleanUp.Replace("\\", "+");
                            NameCleanUp = NameCleanUp.Replace("-", "_");

                            string[] Splits = NameCleanUp.Split('+');
                            if (Splits.Length == 0 || Splits.Length == 1)
                            {
                                NameCleanUp = Splits[0];
                            }
                            else
                            {
                                NameCleanUp = Splits[Splits.Length - 1];
                            }



                            string Temp = "#define " + NameCleanUp;
                            sw.Write(Temp);
                            for (int q = 0; q < 70 - Temp.Length; q++)
                            {
                                sw.Write(" ");
                            }
                            sw.WriteLine(SoundRecords[i].TableIndex);
                            
                            SoundLen = (UInt32)(((double)SoundRecords[i].WavDataLength / (double)SAMPLE_RATE) * (double)PLAY_TIMEOUT_RESOLUTION);


                            Temp = "#define " + NameCleanUp + "_LENGTH";
                            sw.Write(Temp);
                            for (int q = 0; q < 70 - Temp.Length; q++)
                            {
                                sw.Write(" ");
                            }
                            sw.WriteLine(SoundLen);



                        }
                    }
                }
                sw.WriteLine("\r\n");
                sw.WriteLine("#endif");
                sw.Close();
            }

            catch (Exception Ex)
            {
                AssemblyOK = false;
                ErrorOutput += "Error while writing to SoundMapping.h: " + Ex.Message;
            }


            return AssemblyOK;

        }


       

        private bool ProcessLine(String LineIn)
        {  

            string StrippedLine = LineIn.Trim();
           
            //Next Ignore comments

            if(MultiLineCommentStart == false)
            {
                //see if this line starts a multiple line comment
                if (Regex.IsMatch(StrippedLine, @"^\s*/\*.*") == true) 
                {
                    MultiLineCommentStart = true;
                    ProcessorOutput += "Line [" + CurrentLine + "] started a multi-line comment\r\n";
                   
                    //Now, Also Check to see if this is terminated in the same line
                    if (Regex.IsMatch(StrippedLine, @".*\*/\s*$") == true)
                    {
                        MultiLineCommentStart = false;
                        ProcessorOutput += "Line [" + CurrentLine + "] ended a multi-line comment\r\n";
                    }

                    return true;
                }
                else
                {
                   //if we didn't get a multi-line, check for single line
                    if (Regex.IsMatch(StrippedLine, @"^\s*//") == true) 
                        {
                            ProcessorOutput += "Line [" + CurrentLine + "] is a single line comment\r\n";
                            return true;
                        }

                }
             }
             else //See if if have ended
                {
                  if (Regex.IsMatch(StrippedLine, @".*\*/\s*$") == true) 
                      {
                       MultiLineCommentStart = false;
                       ProcessorOutput += "Line [" + CurrentLine + "] ended a multi-line comment\r\n";
                       return true;
                      }
                    else
                  {
                      ProcessorOutput += "Line [" + CurrentLine + "] is part of a multi-line comment\r\n";
                      return true;
                  }

                }


                // Leave out blank lines
                if (StrippedLine.Trim() == "")
                {
                    ProcessorOutput += "Line [" + CurrentLine + "] is blank\r\n";
                    return true;
                }
       

            //Now, see if there is a single line comment with the line with a statment
            string Temp = @"(?<STRIPPED_STATEMENT>.*)//.*";
            if (Regex.IsMatch(StrippedLine, Temp))
            {
                 Match StatementMatch = Regex.Match(StrippedLine, Temp);
                 StrippedLine = StatementMatch.Groups["STRIPPED_STATEMENT"].ToString();

                 ProcessorOutput += "Line [" + CurrentLine + "] had a statement ("+StrippedLine+") with a single line comment\r\n";
            }
           


            //Locate Macros   
                    string MacroScanTemp = @"^\s*\#define\s*(?<MACRO_NAME>[A-Z,_,0-9]*)\s*(?<MACRO_VALUE>.*)";
                    if (Regex.IsMatch(StrippedLine, MacroScanTemp))
                    {
                        Match MacroMatch = Regex.Match(StrippedLine, MacroScanTemp);

                        String MacroName = MacroMatch.Groups["MACRO_NAME"].ToString();
                        String MacroValue = MacroMatch.Groups["MACRO_VALUE"].ToString();
                        ProcessorOutput += "Line [" + CurrentLine + "] has a macro " + MacroName + " with a value of " + MacroValue +"\r\n";
                        
                        MyMacros.Add(new Macro(MacroName,MacroValue));
                        return true;                                 
                    }


            //Replace lines with MACROS defines up until that point
                    Macro[] Macros = new Macro[MyMacros.Count];
                    MyMacros.CopyTo(Macros);
                    for (int i = 0; i < Macros.Length; i++)
                    {

                        if(Regex.IsMatch(StrippedLine, Macros[i].MacroName.Trim()) == true)
                        {
                            StrippedLine = Regex.Replace(StrippedLine, Macros[i].MacroName.Trim(), Macros[i].MacroValue);
                            ProcessorOutput += "Line [" + CurrentLine + "] has a macro replace of " + Macros[i].MacroName + " to " + Macros[i].MacroValue + "\r\n";
                        }
                    }
            
                  //see if the current line has any sounds defined
                    string SoundScanString = @"^\s*(S|s)ound\s*\[(?<SOUND_NUMBER>\s*\d*\s*)\]\s*=\s*(?<SOUND_NAME>.*)\s*;\s*$";
                    if (Regex.IsMatch(StrippedLine, SoundScanString) == true)
                    {
                        Match SoundScanMatch = Regex.Match(StrippedLine, SoundScanString);

                        String SoundNumber = SoundScanMatch.Groups["SOUND_NUMBER"].ToString();
                        String SoundName = SoundScanMatch.Groups["SOUND_NAME"].ToString();

                        ProcessorOutput += "Line [" + CurrentLine + "] has a sound " + SoundName + " at sound number " + SoundNumber + "\r\n";
                        MySoundRecords.Add(new SoundRecord(SoundName, SoundNumber, CurrentLine));
                        return true;
                    }


            ProcessorOutput += "Line [" + CurrentLine + "] has an error - unrecognized statement\r\n";
            ErrorOutput += "Line [" + CurrentLine + "] has an error - unrecognized statement\r\n";

            return false;
        }

        public bool ProcessFile(string FileToProcess)
        {
            CurrentLine = 1;
            StreamReader FileIn;
            bool CleanSyntax = true;

            try
            {
                FileIn = new StreamReader(FileToProcess);
            }
            catch
            {
                ErrorOutput = "Could not open " + FileToProcess + " for processing";
                return false;
            }

            string NextLineToProcess= FileIn.ReadLine();

            while (NextLineToProcess != null)
            {

                if (ProcessLine(NextLineToProcess) == false) 
                {
                    CleanSyntax = false;
                }
                NextLineToProcess = FileIn.ReadLine();
                CurrentLine++;
            }

            FileIn.Close();

            return CleanSyntax;

        }

        public ProfilePackager()
        {
            CurrentLine = 1;
            ProcessorOutput="";
            ErrorOutput="";
        }

    }
    
    public class SoundRecord
    {
        public string FileName="";
        public string Number="";
        public int CodeLine;
        
        public int TableIndex = -1;

        public UInt32 WavDataLength = 0;
        public UInt32 FrontWavPadding = 0; //NEOS Sounds Need padding to next FLASH page
        public UInt32 EndWavPadding = 0; //NEOS Sounds Need padding to next FLASH page


        public SoundRecord()
        {
            FileName = "";
            Number = "";
            CodeLine =0;
            WavDataLength = 0;
            TableIndex = -1;
        }

        public SoundRecord(string SoundFileName,string SoundNumber,int SourceCodeLine)
        {
            this.FileName = SoundFileName;
            this.Number = SoundNumber;
            this.CodeLine = SourceCodeLine;

            TableIndex = -1; 
            WavDataLength = 0;
        }



    }


    public class Macro
    {
        public string MacroName;
        public string MacroValue;

        public Macro()
        {
             MacroName="";
             MacroValue="";
        }

        public Macro(String Name, String Value)
        {
            MacroName = Name;
            MacroValue = Value;
        }
    }


    public class Parameter
    {
        const uint CHAR = 0;
        const uint UNSIGNED_CHAR = 1;
        const uint SHORT = 2;
        const uint UNSIGNED_SHORT = 3;
        const uint INT = 4;
        const uint UNSIGNED_INT = 5;



        public UInt32 BaseAddress = 0;

        

    }

}
