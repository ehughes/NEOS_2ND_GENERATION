using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CANInterfaceManagement;
using System.Threading;

namespace CAN_SNIFFER
{
    public partial class NEOSALLAroundSimulator : Form
    {
        public CANMessageQueue RxCANMessageQueue = new CANMessageQueue(2048);
        public CANMessageQueue TxCANMessageQueue = new CANMessageQueue(2048);

        CANMessage NextMessage = new CANMessage();

        Int16[] ButtonLEDTimer = new short[8];

        NationalInstruments.UI.WindowsForms.Led[] LEDS = new NationalInstruments.UI.WindowsForms.Led[8];
        Thread CANProcessingThread;

        NEOSButtonLED[] MyNEOSButtonLEDS = new NEOSButtonLED[8];
        Label[] HitIndicationLabel = new Label[8];
   
        Image SelectorPanelBackground = new Bitmap("SelectorPanel.bmp");

        ButtonLocation[] SelectorButtonLocation = new ButtonLocation[10];

      


        const byte START_BUTTON_1P = 0;
        const byte START_BUTTON_2P = 1;
        const byte LIGHT_GRABBER = 2;
        const byte FIRE_FIGHTER = 3;
        const byte DOUBLE_DOTS = 4;
        const byte SURROUND_SOUND = 5;
        const byte MARATHON = 6;
        const byte ZIG_ZAG = 7;
        const byte NINJA = 8;
        const byte RODEO = 9;

        const byte  ONE_PLAYER_START_BUTTON_BIT= 	9;
        const byte  TWO_PLAYER_START_BUTTON_BIT=	14;
        const byte  LIGHT_GRABBER_BUTTON_BIT=		5;
        const byte  DOUBLE_DOTS_BUTTON_BIT=			11;
        const byte  SURROUND_SOUND_BUTTON_BIT=		12;
        const byte  RODEO_BUTTON_BIT=				6;
        const byte  MARATHON_BUTTON_BIT= 			13;
        const byte  ZIGZAG_BUTTON_BIT=				8;
        const byte  NINJA_BUTTON_BIT= 				7;
        const byte  FIRE_FIGHTER_BUTTON_BIT= 		10;
        
        //Display upper Left   .26 .319
        //Display Lower Right  .713 .409


        const float SegWidth = 0.053794f;
        const float SegHieght = 0.035f;
        const float SegXPadding = .00326f;
        const float SegYPadding =0.01800f;

        byte[] ScoreSegments = new byte[6];
        PointF [] ScoreSegmentLocation = new PointF[6];


        const Double HitAcceptanceRadius = .013;
        const float ScoreButtonLampRadius = 10;
        ushort ButtonLamps0to15;
        ushort ButtonLamps16to19;


        const byte SEG_A = 0x01;
        const byte SEG_B = 0x02;
        const byte SEG_C = 0x04;
        const byte SEG_D = 0x08;
        const byte SEG_E = 0x10;
        const byte SEG_F = 0x20;
        const byte SEG_G = 0x40;
       


        void InitSelectorPanelData()
        {
            SelectorButtonLocation[START_BUTTON_1P] = new ButtonLocation(.423, .748);
            SelectorButtonLocation[START_BUTTON_2P] = new ButtonLocation(.543, .752);
            SelectorButtonLocation[LIGHT_GRABBER] = new ButtonLocation(.486, .6);
            SelectorButtonLocation[FIRE_FIGHTER] = new ButtonLocation(.603, .636);
            SelectorButtonLocation[DOUBLE_DOTS] = new ButtonLocation(.65, .714);
            SelectorButtonLocation[SURROUND_SOUND] = new ButtonLocation(.6, .7901);
            SelectorButtonLocation[MARATHON] = new ButtonLocation(.483, .821);
            SelectorButtonLocation[ZIG_ZAG] = new ButtonLocation(.3666, .785);
            SelectorButtonLocation[NINJA] = new ButtonLocation(.32, .707);
            SelectorButtonLocation[RODEO] = new ButtonLocation(.37, .631);


            ScoreSegmentLocation[0] = new PointF(.26f, .320f);
            ScoreSegmentLocation[1] = new PointF(.324f, .320f);
            ScoreSegmentLocation[2] = new PointF(.389f, .320f);
            ScoreSegmentLocation[3] = new PointF(.518f, .320f);
            ScoreSegmentLocation[4] = new PointF(.583f, .320f);
            ScoreSegmentLocation[5] = new PointF(.648f, .320f);

            ScoreSegments[0] = SEG_A; 
            ScoreSegments[1] = SEG_B; 
            ScoreSegments[2] = SEG_C; 
            ScoreSegments[3] = SEG_D; 
            ScoreSegments[4] = SEG_E; 
            ScoreSegments[5] = SEG_G ;
            



        }

      
        
        public void Terminate()
        {
            CANProcessingThread.Abort();
            
        }

        public NEOSALLAroundSimulator()
        {
            InitializeComponent();

            LEDS[0] = BUTTON_LED0;
            LEDS[1] = BUTTON_LED1;
            LEDS[2] = BUTTON_LED2;
            LEDS[3] = BUTTON_LED3;
            LEDS[4] = BUTTON_LED4;
            LEDS[5] = BUTTON_LED5;
            LEDS[6] = BUTTON_LED6;
            LEDS[7] = BUTTON_LED7;


            LEDS[0].Click +=new EventHandler(NEOSButtonHit_0);
            LEDS[1].Click += new EventHandler(NEOSButtonHit_1);
            LEDS[2].Click += new EventHandler(NEOSButtonHit_2);
            LEDS[3].Click += new EventHandler(NEOSButtonHit_3);
            LEDS[4].Click += new EventHandler(NEOSButtonHit_4);
            LEDS[5].Click += new EventHandler(NEOSButtonHit_5);
            LEDS[6].Click += new EventHandler(NEOSButtonHit_6);
            LEDS[7].Click += new EventHandler(NEOSButtonHit_7);
          

            for(int i=0;i<8;i++)
            {
                ButtonLEDTimer[i] = 0;
                MyNEOSButtonLEDS[i] = new NEOSButtonLED();
                HitIndicationLabel[i] = new  Label();
                HitIndicationLabel[i].Text = "";
                HitIndicationLabel[i].Location = new Point(LEDS[i].Location.X+5,LEDS[i].Location.Y-15);
                this.Controls.Add(HitIndicationLabel[i]);
            }

            this.RxCANMessageQueue.Clear();
            CANProcessingThread = new Thread(new ThreadStart(CANRxProcess));
            CANProcessingThread.Priority = ThreadPriority.Highest;
            CANProcessingThread.Start();

            SelectorPanel.Paint+=new PaintEventHandler(SelectorPanel_Paint);
            InitSelectorPanelData();
        }

        void PaintSelectorButtonLamps(ushort ButtonLamps0to15, ref PaintEventArgs e)
        {
            
            
                if ((ButtonLamps0to15 & (1 << ONE_PLAYER_START_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[START_BUTTON_1P].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[START_BUTTON_1P].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << TWO_PLAYER_START_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[START_BUTTON_2P].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[START_BUTTON_2P].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << LIGHT_GRABBER_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[LIGHT_GRABBER].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[LIGHT_GRABBER].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << DOUBLE_DOTS_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[DOUBLE_DOTS].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[DOUBLE_DOTS].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << SURROUND_SOUND_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[SURROUND_SOUND].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[SURROUND_SOUND].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << RODEO_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[RODEO].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[RODEO].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << MARATHON_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[MARATHON].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[MARATHON].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << ZIGZAG_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[ZIG_ZAG].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[ZIG_ZAG].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << NINJA_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[NINJA].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[NINJA].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }

                if ((ButtonLamps0to15 & (1 << FIRE_FIGHTER_BUTTON_BIT)) > 0)
                {
                    e.Graphics.DrawEllipse(new Pen(Brushes.Red, 2),
                                               (float)(SelectorButtonLocation[FIRE_FIGHTER].X * SelectorPanel.Width) - (ScoreButtonLampRadius / 2),
                                               (float)(SelectorButtonLocation[FIRE_FIGHTER].Y * SelectorPanel.Height) - (ScoreButtonLampRadius / 2),
                                               ScoreButtonLampRadius,
                                               ScoreButtonLampRadius);
                }
            
        }


        void PaintSevenSegmentDigit(byte SegData, ref PaintEventArgs e, PointF ReferenceLocation, Size RescaleSize, Brush SegmentBrush)
        {
            float RescaleFactorX = RescaleSize.Width;
            float RescaleFactorY = RescaleSize.Height;


            float AngleIn = (float)(SegWidth * .12);

            Pen SegPen = new Pen(SegmentBrush, 2);
          
                if ((SegData & SEG_A) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + (2 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2));
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth + (2 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2));
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_B) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth + (2 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2));
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_C) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth, ReferenceLocation.Y + (SegYPadding / 2) + (2 * SegHieght));
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_D) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth, ReferenceLocation.Y + (SegYPadding / 2) + (2 * SegHieght));
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2), ReferenceLocation.Y + (SegYPadding / 2) + (2 * SegHieght));
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_E) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2), ReferenceLocation.Y + (SegYPadding / 2) + (2 * SegHieght));
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_F) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + (2 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2));
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }

                if ((SegData & SEG_G) > 0)
                {
                    PointF Start = new PointF(ReferenceLocation.X + (SegXPadding / 2) + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    PointF Stop = new PointF(ReferenceLocation.X + (SegXPadding / 2) + SegWidth + (1 * AngleIn), ReferenceLocation.Y + (SegYPadding / 2) + SegHieght);
                    e.Graphics.DrawLine(SegPen, RescalePointF(Start, RescaleFactorX, RescaleFactorY), RescalePointF(Stop, RescaleFactorX, RescaleFactorY));
                }
           
        }

        PointF AddPoint(PointF P1, PointF P2)
        {
            return new PointF(P1.X + P2.X, P1.Y + P2.Y);

        }

        PointF SubtractPoint(PointF P1, PointF P2)
        {
            return new PointF(P1.X - P2.X, P1.Y - P2.Y);
        }

        PointF MultiplyPoint(PointF P1, float P2)
        {
            return new PointF(P1.X * P2, P1.Y * P2);
        }

        Point RescalePointF(PointF P1, float RX, float RY)
        {
            return new Point((int)(P1.X * RX), (int)(P1.Y * RY));

        }

        void  SelectorPanel_Paint(object sender, PaintEventArgs e)
        {
 	        Point P1 = new Point(0, 0);
            Rectangle SrcRec = new Rectangle(P1, new Size(SelectorPanelBackground.Size.Width,SelectorPanelBackground.Size.Height));
            Rectangle DestRec = new Rectangle(P1,new Size(SelectorPanel.Size.Width,SelectorPanel.Size.Height));

            e.Graphics.DrawImage(SelectorPanelBackground, DestRec, SrcRec, GraphicsUnit.Pixel);

           
            PaintSelectorButtonLamps(ButtonLamps0to15,ref e);
            lock (ScoreSegments)
            {
                for (int i = 0; i < 3; i++)
                {
                    PaintSevenSegmentDigit(ScoreSegments[i], ref  e, ScoreSegmentLocation[i], SelectorPanel.Size, Brushes.Red);
                }
                for (int i = 3; i < 6; i++)
                {
                    PaintSevenSegmentDigit(ScoreSegments[i], ref  e, ScoreSegmentLocation[i], SelectorPanel.Size, Brushes.SpringGreen);
                }
            }
        }

        public void TransmitSelectorButtonCode(byte SelectorButton)
        {
            NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Node = 248;

            switch(SelectorButton)
            {

                           

                case START_BUTTON_1P:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << ONE_PLAYER_START_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case START_BUTTON_2P:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << TWO_PLAYER_START_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case LIGHT_GRABBER:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << LIGHT_GRABBER_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case FIRE_FIGHTER:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << FIRE_FIGHTER_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case DOUBLE_DOTS:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << DOUBLE_DOTS_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case SURROUND_SOUND:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << SURROUND_SOUND_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case MARATHON:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << MARATHON_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case ZIG_ZAG:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << ZIGZAG_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case NINJA:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << NINJA_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;
                case RODEO:
                    NEOSMessages.SCORE_BUTTONS_CHANGED_PARAMS.Buttons1to16 = (ushort)(1 << RODEO_BUTTON_BIT);
                     TxCANMessageQueue.Enqueue(NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.SCORE_BUTTONS_CHANGED));
                    break;

            }
        }

        private void SelectorPanel_MouseDown(object sender, MouseEventArgs e)
        {

           ButtonLocation ClickLocation = new ButtonLocation((double)e.X/(double)SelectorPanel.Size.Width,(double)e.Y/(double)SelectorPanel.Size.Height);

            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[START_BUTTON_1P])<HitAcceptanceRadius)
                TransmitSelectorButtonCode(START_BUTTON_1P);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[START_BUTTON_2P])<HitAcceptanceRadius)
                TransmitSelectorButtonCode(START_BUTTON_2P);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[LIGHT_GRABBER])<HitAcceptanceRadius)
                TransmitSelectorButtonCode(LIGHT_GRABBER);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[FIRE_FIGHTER])<HitAcceptanceRadius) 
               TransmitSelectorButtonCode(FIRE_FIGHTER);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[DOUBLE_DOTS])<HitAcceptanceRadius)
                TransmitSelectorButtonCode(DOUBLE_DOTS);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[SURROUND_SOUND])<HitAcceptanceRadius) 
                TransmitSelectorButtonCode(SURROUND_SOUND);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[MARATHON])<HitAcceptanceRadius)
                TransmitSelectorButtonCode(MARATHON);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[ZIG_ZAG])<HitAcceptanceRadius)
               TransmitSelectorButtonCode(ZIG_ZAG);
            if(ButtonLocation.Distance(ClickLocation,SelectorButtonLocation[NINJA])<HitAcceptanceRadius) 
                TransmitSelectorButtonCode(NINJA);
            if (ButtonLocation.Distance(ClickLocation, SelectorButtonLocation[RODEO]) < HitAcceptanceRadius)
                TransmitSelectorButtonCode(RODEO);


        }
        void NEOSButtonHit_0(object sender, EventArgs e)
        {
            NEOSButtonHit(0);
        }
        void NEOSButtonHit_1(object sender, EventArgs e)
        {
            NEOSButtonHit(1);
        }
        void NEOSButtonHit_2(object sender, EventArgs e)
        {
            NEOSButtonHit(2);
        }
        void NEOSButtonHit_3(object sender, EventArgs e)
        {
            NEOSButtonHit(3);
        }
        void NEOSButtonHit_4(object sender, EventArgs e)
        {
            NEOSButtonHit(4);
        }
        void NEOSButtonHit_5(object sender, EventArgs e)
        {
            NEOSButtonHit(5);
        }
        void NEOSButtonHit_6(object sender, EventArgs e)
        {
            NEOSButtonHit(6);
        }
        void NEOSButtonHit_7(object sender, EventArgs e)
        {
            NEOSButtonHit(7);
        }

        void NEOSButtonHit(byte Button)
        {
            NEOSMessages.BUTTON_PRESS_PARAMS.Node = Button;
            CANMessage Outgoing = NEOSMessages.AssembleNEOSCANMessage(NEOSMessages.BUTTON_PRESSED);

            TxCANMessageQueue.Enqueue(Outgoing);

        }

        public void CANRxProcess()
        {
            while (true)
            {
                Thread.Sleep(1);
                lock (this.RxCANMessageQueue)
                {
                    for (int q = 0; q < this.RxCANMessageQueue.GetCount(); q++)
                    {
                        RxCANMessageQueue.Dequeue(ref NextMessage);

                        if (NextMessage != null)
                        {
                            switch (NextMessage.CANId)
                            {

                                case NEOSMessages.SEND_SCORE_SEGMENT_DATA:
                                    lock (ScoreSegments)
                                    {
                                        ScoreSegments[0] = NextMessage.CANData[2];
                                        ScoreSegments[1] = NextMessage.CANData[3];
                                        ScoreSegments[2] = NextMessage.CANData[4];
                                        ScoreSegments[3] = NextMessage.CANData[5];
                                        ScoreSegments[4] = NextMessage.CANData[6];
                                        ScoreSegments[5] = NextMessage.CANData[7];
                                        SelectorPanel.Invalidate();
                                    }
                                    break;

                                case NEOSMessages.SCORE_LED_COMMAND:

                                       ButtonLamps0to15 = (ushort)(NextMessage.CANData[2] + (NextMessage.CANData[3] << 8));
                                       ButtonLamps16to19 = (ushort)(NextMessage.CANData[4] + (NextMessage.CANData[5] << 8));
                                       SelectorPanel.Invalidate();



                                    break;

                                case NEOSMessages.LED_COMMAND:


                                    byte Red = (byte)((NextMessage.CANData[0] & 0x3f) );
                                    byte Green = (byte)(((((NextMessage.CANData[1] & 0xf) << 2) | ((NextMessage.CANData[0] & 0xC0) >> 2)) )*0x3f);

                                    Red = (byte)(Red * 4);

                                    Green = (byte)(Green * 4);

                                  /*  if (Red > 0)
                                    {
                                        Int32 temp = Red + 16;
                                        if (temp > 255)
                                        {
                                            Red = (byte)255;
                                        }
                                        else
                                        {
                                            Red = (byte)temp;
                                        }
                                    }

                                    if (Green > 0)
                                    {
                                        Int32 temp = Green + 16;
                                        if (temp > 255)
                                        {
                                            Green = (byte)255;
                                        }
                                        else
                                        {
                                            Green = (byte)temp;
                                        }
                                    }*/

                                    short LEDTimeOut = (short)(((NextMessage.CANData[4] >> 4) & 0xf) | ((NextMessage.CANData[5] & 0x3f) << 4));



                                    byte RedTarget = (byte)(NextMessage.CANData[2] & 0xFC);
                                    byte GreenTarget = (byte)((NextMessage.CANData[3] & 0x3F) << 2);

                                    short LEDFadeTime = (short)(((NextMessage.CANData[5] & 0xC0) >> 6) | (NextMessage.CANData[6] << 2));

                                    double RedFadeStep;
                                    double GreenFadeStep;


                                    if (LEDFadeTime > 0)
                                    {
                                        RedFadeStep = (RedTarget - Red) / (LEDFadeTime);
                                        GreenFadeStep = (GreenTarget - Green) / (LEDFadeTime);
                                    }
                                    else
                                    {
                                        RedFadeStep = 0;
                                        GreenFadeStep = 0;

                                    }



                                    if (NextMessage.CANData[7] == 0xff)
                                    {
                                        for (int j = 0; j < 8; j++)
                                        {
                                            MyNEOSButtonLEDS[j].LEDTimer = LEDTimeOut;
                                            if (LEDTimeOut > 0)
                                            {
                                                MyNEOSButtonLEDS[j].NoTimeOut = false;
                                            }
                                            else
                                            {
                                                MyNEOSButtonLEDS[j].NoTimeOut = true;
                                            }


                                            MyNEOSButtonLEDS[j].Red = (double)Red;
                                            MyNEOSButtonLEDS[j].Green = (double)Green;
                                            MyNEOSButtonLEDS[j].FadeStepsRemaining = LEDFadeTime;
                                            MyNEOSButtonLEDS[j].RedFadeStep = (double)RedFadeStep;
                                            MyNEOSButtonLEDS[j].GreenFadeStep = (double)GreenFadeStep;


                                        }
                                    }
                                    else if (NextMessage.CANData[7] < 8)
                                    {
                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].LEDTimer = LEDTimeOut;

                                        if (LEDTimeOut > 0)
                                        {
                                            MyNEOSButtonLEDS[NextMessage.CANData[7]].NoTimeOut = false;
                                        }
                                        else
                                        {
                                            MyNEOSButtonLEDS[NextMessage.CANData[7]].NoTimeOut = true;
                                        }


                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].Red = (double)Red;
                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].Green = (double)Green;
                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].FadeStepsRemaining = LEDFadeTime;
                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].RedFadeStep = (double)RedFadeStep;
                                        MyNEOSButtonLEDS[NextMessage.CANData[7]].GreenFadeStep = (double)GreenFadeStep;
                                    }
                                    break;


                                case NEOSMessages.RESET_SLAVES:

                                    for (int i = 0; i < 8; i++)
                                    {
                                        MyNEOSButtonLEDS[i].NoTimeOut = false;
                                        MyNEOSButtonLEDS[i].LEDTimer = 0;
                                        MyNEOSButtonLEDS[i].Red = 0;
                                        MyNEOSButtonLEDS[i].Green = 0;
                                        MyNEOSButtonLEDS[i].FadeStepsRemaining = 0;
                                        MyNEOSButtonLEDS[i].RedFadeStep = 0;
                                        MyNEOSButtonLEDS[i].GreenFadeStep = 0;

                                    }

                                    break;

                                case NEOSMessages.BUTTON_PRESSED:

                                    if (NextMessage.CANData[0] < 8)
                                    {

                                        MyNEOSButtonLEDS[NextMessage.CANData[0]].BeenHit = true;
                                        MyNEOSButtonLEDS[NextMessage.CANData[0]].HitDisplayTimer = 25;


                                    }


                                    break;

                                default:
                                    break;

                            }
                        }
                    }
                }
            }
        }

      
        private void NEOSCtrlPanel_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
        }

        private void led5_StateChanged(object sender, NationalInstruments.UI.ActionEventArgs e)
        {

        }

        private void led2_StateChanged(object sender, NationalInstruments.UI.ActionEventArgs e)
        {

        }

        private void NEOSCtrlPanel_Load(object sender, EventArgs e)
        {

        }

        private void led6_StateChanged(object sender, NationalInstruments.UI.ActionEventArgs e)
        {

        }
          

        private void ButtonStateTimer_Tick_1(object sender, EventArgs e)
        {
          
                LEDTimeoutCheck();
                PerformFade();
                HitIndicator();
        }

        void HitIndicator()
        {
            for (int i = 0; i < 8; i++)
            {
                if (MyNEOSButtonLEDS[i].BeenHit == true)
                {
                    if (MyNEOSButtonLEDS[i].HitDisplayTimer > 0)
                    {
                        MyNEOSButtonLEDS[i].HitDisplayTimer--;
                        HitIndicationLabel[i].Text = "Hit!";

                    }
                    else
                    {
                        MyNEOSButtonLEDS[i].BeenHit = false;

                        HitIndicationLabel[i].Text ="";
                    }

                }
            }

        }
        void LEDTimeoutCheck()
        {
            for (int i = 0; i < 8; i++)
            {
                if (MyNEOSButtonLEDS[i].NoTimeOut == false)
                {
                    if (MyNEOSButtonLEDS[i].LEDTimer > 1)
                    {
                        MyNEOSButtonLEDS[i].LEDTimer--;
                    }
                    else if (MyNEOSButtonLEDS[i].LEDTimer == 1)
                    {
                        if (MyNEOSButtonLEDS[i].Red > 0)
                        {
                            MyNEOSButtonLEDS[i].Red = 0;
                        }
                        if (MyNEOSButtonLEDS[i].Green > 0)
                        {
                            MyNEOSButtonLEDS[i].Green = 0;
                        }
                    }

                }
                else
                {

                }
            }

        }

        void PerformFade()
        {
            for (int j = 0; j < 8; j++)
            {
                if (MyNEOSButtonLEDS[j].FadeStepsRemaining > 0)
                {
                    MyNEOSButtonLEDS[j].FadeStepsRemaining--;

                    if (MyNEOSButtonLEDS[j].RedFadeStep != 0)
                    {
                        MyNEOSButtonLEDS[j].Red += MyNEOSButtonLEDS[j].RedFadeStep;

                        if (MyNEOSButtonLEDS[j].Red > 255.0)
                        {
                            MyNEOSButtonLEDS[j].Red = 255.0;
                        }
                        if (MyNEOSButtonLEDS[j].Red < 0)
                        {
                            MyNEOSButtonLEDS[j].Red = 0;
                        }

                    }

                    if (MyNEOSButtonLEDS[j].GreenFadeStep != 0)
                    {
                        MyNEOSButtonLEDS[j].Green += MyNEOSButtonLEDS[j].GreenFadeStep;
                        if (MyNEOSButtonLEDS[j].Green > 255.0)
                        {
                            MyNEOSButtonLEDS[j].Green = 255.0;
                        }
                        if (MyNEOSButtonLEDS[j].Green < 0)
                        {
                            MyNEOSButtonLEDS[j].Green = 0;
                        }
                    }
                }
            }
        }

        private void UIUpdateTimer_Tick(object sender, EventArgs e)
        {
            for (int i = 0; i < 8; i++)
            {
              

                   // if ( MyNEOSButtonLEDS[i].State== true)
                   // {
                        LEDS[i].Value = true;
                   // }
                   // else
                  //  {
                    //    LEDS[i].Value = false;
                   // }

                    LEDS[i].OnColor = Color.FromArgb((byte)MyNEOSButtonLEDS[i].Red, (byte)MyNEOSButtonLEDS[i].Green, 0);
                            
            }
            SelectorPanel.Invalidate();
        }

        private void CANRxTimer_Tick(object sender, EventArgs e)
        {
            //CANRxProcess();
        }

      
        class ButtonLocation
        {
            public double X;
            public double Y;

            public  ButtonLocation(double InitX,double InitY)
            {
                X = InitX;
                Y= InitY;
            }
            public ButtonLocation()
            {
                X = 0;
                Y= 0;
            }

            public static double Distance(ButtonLocation P1, ButtonLocation P2)
            {
                return Math.Sqrt(Math.Pow((P2.X - P1.X),2) + Math.Pow((P2.Y - P1.Y),2));
            }
        }



    }

 

    public class NEOSButtonLED
    {
        public double Red = 0;
        public double Green = 0;
        public double RedFadeStep = 0;
        public double GreenFadeStep = 0;
        public bool NoTimeOut = false;     
        public short LEDTimer = 0;
        public short FadeStepsRemaining=0;

        public bool BeenHit = true;
        public int HitDisplayTimer = 0;
    }

   

}