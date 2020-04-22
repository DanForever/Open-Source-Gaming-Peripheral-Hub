using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MahApps.Metro.Controls;
using Managed.HID;

namespace Hub
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        private List<Managed.HID.Device> Devices = new List<Managed.HID.Device>();

        public MainWindow()
        {
            InitializeComponent();

            var enumerator = new Managed.HID.Enumerator();

            foreach(var pathCollection in enumerator.Collection )
            {
                var device = new Managed.HID.Device();

                if(device.Open(pathCollection))
                {
                    Devices.Add( device );
                }
            }

            TestGrid.ItemsSource = Devices;
        }
    }
}
