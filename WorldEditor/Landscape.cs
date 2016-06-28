using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace WorldEditor
{
    public class Landscape
    {
        public string Name { get; set; }
        public int Attack { get; set; }
        public int Defense { get; set; }
        public int Effect { get; set; }
        public Color LandColor { get; set; }
        public Landscape(string p_name, int p_attack, int p_defense, int p_effect, Color p_color)
        {
            Name = p_name;
            Attack = p_attack;
            Defense = p_defense;
            Effect = p_effect;
            LandColor = p_color;
        }
    }
}
