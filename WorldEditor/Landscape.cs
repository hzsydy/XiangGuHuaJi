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

        public Landscape()
            : this("", 100, 100, 0, Color.White)
        {
            ;
        }

        public Landscape Clone()
        {
            return new Landscape(Name, Attack, Defense, Effect, LandColor);
        }

        public override bool Equals(object obj)
        {
            Landscape ls = (Landscape)obj;
            return (
                ls.Defense == Defense &&
                ls.Attack == Attack &&
                ls.Effect == Effect &&
                ls.Name == Name
            );
        }
    }
}
