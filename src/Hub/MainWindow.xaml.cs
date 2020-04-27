namespace Hub
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();

            var enumerator = new Managed.HIDPP.Enumerator();

            TestGrid.ItemsSource = enumerator;
        }
    }
}
