using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace TestScriptGenerator {
  class Program {
    static int testId = 1;

    static void Main(string[] args) {
      XmlWriterSettings settings = new XmlWriterSettings();
      settings.Indent = true;
      settings.IndentChars = "\t";
      XmlWriter writer = XmlWriter.Create("test_params.xml", settings);

      writer.WriteStartDocument();
      writer.WriteStartElement("test_list");

      for (int type = 0; type < 4; type++) {
        switch (type) {
          case 0:
            writer.WriteComment(" Explict Euler block ");
            break;
          case 1:
            writer.WriteComment(" Verlet block ");
            break;
          case 2:
            writer.WriteComment(" RK4 block ");
            break;
          case 3:
            writer.WriteComment(" Midpoint block ");
            break;
          default:
            break;
        }

        for (int size = 50; size <= 300; size += 50) {
          writer.WriteComment(" " + size + " x " + size + " ");

          for (int timeStep = 0; timeStep <= 20; timeStep += 5) {
            writer.WriteStartElement("test"); // <test>
            writer.WriteAttributeString("id", XmlConvert.ToString(testId++));

            writer.WriteStartElement("integrator"); // <integrator>
            writer.WriteAttributeString("type", XmlConvert.ToString(type));
            writer.WriteAttributeString("time_step", XmlConvert.ToString((float) timeStep == 0 ? 1 : timeStep));
            writer.WriteEndElement(); // </integrator>

            writer.WriteStartElement("cloth_params"); // <cloth_params>
            writer.WriteAttributeString("height", "10.0");
            writer.WriteAttributeString("width", "10.0");
            writer.WriteAttributeString("mass", "100.0");
            writer.WriteAttributeString("wind_constant", "0.5");

            writer.WriteStartElement("top_left_position"); // <top_left_position>
            writer.WriteAttributeString("x", "-10.0");
            writer.WriteAttributeString("y", "10.0");
            writer.WriteAttributeString("z", "10.0");
            writer.WriteEndElement(); // </top_left_position>

            writer.WriteStartElement("mesh_size"); // <mesh_size>
            writer.WriteAttributeString("rows", XmlConvert.ToString(size));
            writer.WriteAttributeString("columns", XmlConvert.ToString(size));
            writer.WriteEndElement(); // </mesh_size>

            writer.WriteStartElement("structural"); // <structural>
            writer.WriteAttributeString("spring_coefficient", "20.0");
            writer.WriteAttributeString("damping_coefficient", "7.5");
            writer.WriteEndElement(); // </structural>

            writer.WriteStartElement("shear"); // <shear>
            writer.WriteAttributeString("spring_coefficient", "20.0");
            writer.WriteAttributeString("damping_coefficient", "7.5");
            writer.WriteEndElement(); // </shear>

            writer.WriteStartElement("flexion"); // <flexion>
            writer.WriteAttributeString("spring_coefficient", "5.0");
            writer.WriteAttributeString("damping_coefficient", "2.5");
            writer.WriteEndElement(); // </flexion>

            writer.WriteStartElement("wind_direction"); // <wind_direction>
            writer.WriteAttributeString("x", "0.0");
            writer.WriteAttributeString("y", "0.0");
            writer.WriteAttributeString("z", "1.0");
            writer.WriteEndElement(); // </wind_direction>

            writer.WriteEndElement(); // </cloth_params>

            writer.WriteEndElement(); // </test>
          }
        }
      }

      writer.WriteEndElement(); // </test_list>
      writer.Close();
    }
  }
}