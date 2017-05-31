using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CartoTypeDemo
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            this.Controls.Add(m_status_bar);

            string map_file = Application.StartupPath + "/../../../../data/boston.ctm1";
            string style_file = Application.StartupPath + "/../../../../data/standard.ctstyle";
            string font_path = Application.StartupPath + "/../../../../data/";
            if (!System.IO.File.Exists(map_file)) // if we're running in an ordinary source tree, not an SDK
            {
                map_file = Application.StartupPath + "/../../../../../../test/data/ctm1/boston.ctm1";
                style_file = Application.StartupPath + "/../../../../../../style/standard.ctstyle";
                font_path = Application.StartupPath + "/../../../../../../font/";
            }
            m_framework = new CartoType.Framework(map_file,
                                                  style_file,
                                                  font_path + "DejaVuSans.ttf",
                                                  this.ClientSize.Width,
                                                  this.ClientSize.Height);
            m_framework.LoadFont(font_path + "DejaVuSans-Bold.ttf");
            m_framework.LoadFont(font_path + "DejaVuSerif.ttf");
            m_framework.LoadFont(font_path + "DejaVuSerif-Italic.ttf");
            m_framework.SetResolutionDpi(144);
            m_framework.SetScaleDenominator(50000);
            long id = 0;
            m_framework.InsertPointMapObject(m_framework.GetMemoryMapHandle(),"amenity/minor",
                                             -71.0824,42.3527,
                                             CartoType.CoordType.Degree,
                                             "Graham's Pub",
                                             CartoType.Util.IntAttribute("pub",0),ref id,false);
            m_framework.SetViewCenter(-71.0824, 42.3527, CartoType.CoordType.Degree);
            m_framework.SetFollowMode(CartoType.FollowMode.LocationHeading);
            Text = m_framework.DataSetName();
        }

        private CartoType.Framework m_framework;
        private bool m_map_drag_enabled;
        private int m_map_drag_offset_x;
        private int m_map_drag_offset_y;
        private int m_map_drag_anchor_x;
        private int m_map_drag_anchor_y;
        private Graphics m_map_drag_graphics;
        private CartoType.Point m_last_point = new CartoType.Point();
        private CartoType.Turn m_first_turn = new CartoType.Turn();
        private CartoType.Turn m_second_turn = new CartoType.Turn();
        private StatusBar m_status_bar = new StatusBar();

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }

        private void Draw(Graphics aGraphics)
        {
            Text = m_framework.DataSetName() + " 1:" + (int)m_framework.ScaleDenominator();
            
            if (m_map_drag_enabled)
            {
                if (m_map_drag_offset_x > 0)
                    aGraphics.FillRectangle(Brushes.White, 0, 0, m_map_drag_offset_x, this.ClientRectangle.Height);
                else if (m_map_drag_offset_x < 0)
                    aGraphics.FillRectangle(Brushes.White, this.ClientRectangle.Width + m_map_drag_offset_x, 0, -m_map_drag_offset_x, this.ClientRectangle.Height);
                if (m_map_drag_offset_y > 0)
                    aGraphics.FillRectangle(Brushes.White, 0, 0, this.ClientRectangle.Width, m_map_drag_offset_y);
                else if (m_map_drag_offset_y < 0)
                    aGraphics.FillRectangle(Brushes.White, 0, this.ClientRectangle.Height + m_map_drag_offset_y, this.ClientRectangle.Width, -m_map_drag_offset_y);
            }

            aGraphics.DrawImageUnscaled(m_framework.MapBitmap(), m_map_drag_offset_x, m_map_drag_offset_y);
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            // do nothing: the whole window is drawn by Form1_Paint
        }

        private void Form1_KeyPress(object sender, KeyPressEventArgs e)
        {
            switch (e.KeyChar)
            {
                // Press 'i' to zoom in.
                case 'i':
                    m_framework.Zoom(2);
                    Invalidate();
                    break;

                // Press 'o' to zoom out.
                case 'o':
                    m_framework.Zoom(0.5);
                    Invalidate();
                    break;

                // Press 'r' to rotate right.
                case 'r':
                    m_framework.Rotate(10);
                    Invalidate();
                    break;

                // Press 'l' to rotate left.
                case 'l':
                    m_framework.Rotate(-10);
                    Invalidate();
                    break;

                // Press 'p' to toggle perspective mode.
                case 'p':
                    m_framework.SetPerspective(!m_framework.Perspective());
                    Invalidate();
                    break;
            }
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            switch (keyData)
            {
                case Keys.Left:
                    m_framework.Pan(-50,0);
                    Invalidate();
                    return true;
                case Keys.Right:
                    m_framework.Pan(50,0);
                    Invalidate();
                    return true;
                case Keys.Up:
                    m_framework.Pan(0,-50);
                    Invalidate();
                    return true;
                case Keys.Down:
                    m_framework.Pan(0,50);
                    Invalidate();
                    return true;
            }
            return false;
        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                m_map_drag_enabled = true;
                m_map_drag_anchor_x = e.X;
                m_map_drag_anchor_y = e.Y;
                m_map_drag_graphics = CreateGraphics();
            }
        }

        private void Navigate(int aValidity,double aTime,double aLong,double aLat,double aSpeed,double aBearing, double aHeight)
        {
            m_framework.Navigate(aValidity,aTime,aLong,aLat,aSpeed,aBearing,aHeight);
            m_framework.GetFirstTurn(m_first_turn);
            m_framework.GetSecondTurn(m_second_turn);
            String message = "";

            switch (m_framework.GetNavigationState())
            {
                case CartoType.NavigationState.None:
                    break;

                case CartoType.NavigationState.Turn:
                    message = m_first_turn.TurnCommand() + " after " + (int)m_first_turn.m_distance + "m";
 
                    if (!m_second_turn.m_continue)
                        message += " then " + m_second_turn.TurnCommand() + " after " + (int)m_second_turn.m_distance + "m";
                    break;

                case CartoType.NavigationState.TurnRound:
                    message = "turn round at the next safe and legal opportunity";
                    break;

                case CartoType.NavigationState.NewRoute:
                    message = "calculating a new route";
                    break;

                case CartoType.NavigationState.Arrival:
                    message = "arriving after " + (int)m_first_turn.m_distance + "m";
                    break;

                case CartoType.NavigationState.OffRoute:
                    message = "off route";
                    break;

            }

            m_status_bar.Text = message;
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {

            if (e.Button == MouseButtons.Left)
            {
                m_map_drag_offset_x = e.X - m_map_drag_anchor_x;
                m_map_drag_offset_y = e.Y - m_map_drag_anchor_y;
                m_map_drag_enabled = false;
                if (m_map_drag_graphics != null)
                    m_map_drag_graphics.Dispose();
                m_map_drag_graphics = null;
                m_framework.Pan(-m_map_drag_offset_x, -m_map_drag_offset_y);

                // Simulate a navigation position if there was no drag.
                //if (m_map_drag_offset_x == 0 && m_map_drag_offset_y == 0)
                //{
                //    double time = (double)DateTime.Now.Ticks / 10000000.0;
                //    int validity = (int)CartoType.ValidityFlag.Position | (int)CartoType.ValidityFlag.Time;
                //    double[] coord = { e.X, e.Y };
                //    m_framework.ConvertCoords(coord,CartoType.CoordType.Screen,CartoType.CoordType.Degree);
                //    Navigate(validity, time, coord[0], coord[1], 0, 0, 0);
                //}

                m_map_drag_offset_x = 0;
                m_map_drag_offset_y = 0;
                Invalidate();
            }
            
            // Right-click calculates a route between the last point and this point.
            else if (e.Button == MouseButtons.Right)
            {
                double[] coord = { e.X, e.Y };
                m_framework.ConvertCoords(coord, CartoType.CoordType.Screen,CartoType.CoordType.Degree);
                if (m_last_point.X != 0 && m_last_point.Y != 0)
                {
                    // Delete the previous route, which consists of object IDs 0, 1, and 2.
                    m_framework.StartNavigation(m_last_point.X, m_last_point.Y, CartoType.CoordType.Degree,coord[0],coord[1],CartoType.CoordType.Degree);
                    
                    Invalidate();
                }
                m_last_point.X = coord[0];
                m_last_point.Y = coord[1];
            }
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                m_map_drag_offset_x = e.X - m_map_drag_anchor_x;
                m_map_drag_offset_y = e.Y - m_map_drag_anchor_y;
                Draw(m_map_drag_graphics);
            }
        }

        private void Form1_MouseWheel(object sender, MouseEventArgs e)
        {
            int zoom_count = e.Delta / 120;
            double zoom = Math.Sqrt(2);
            if (zoom_count == 0)
                zoom_count = e.Delta >= 0 ? 1 : -1;
            zoom = Math.Pow(zoom,zoom_count);
            m_framework.Zoom(zoom);
            Invalidate();
        }

        private void Form1_ClientSizeChanged(object sender, EventArgs e)
        {
            if (!this.ClientSize.IsEmpty && m_framework != null)
            {
                m_framework.Resize(this.ClientSize.Width, this.ClientSize.Height);
                Invalidate();
            }
        }

    }
}
