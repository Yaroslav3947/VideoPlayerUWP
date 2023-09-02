using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using VideoPlayerWrapper;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace VideoPlayerUWP {
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page {
        public MainPage() {
            this.InitializeComponent();

            Window.Current.SizeChanged += Window_SizeChanged;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            base.OnNavigatedTo(e);
            mySwapChainPanel.OpenURL("VideoMusk30fps.mp4");
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e) {
        }

        private void Window_SizeChanged(object sender,WindowSizeChangedEventArgs e) {
            double newWidth = e.Size.Width;
            double newHeight = e.Size.Height - 40; ////TODO: get the actual size of bottom panel


            mySwapChainPanel.ResizeSwapChainPanel(newWidth,newHeight);
        }

        private void PlayPauseButton_Click(object sender,RoutedEventArgs e) {

        }

        private void MuteButton_Click_(object sender,RoutedEventArgs e) {

        }

        private void videoSlider_ValueChanged(object sender,RangeBaseValueChangedEventArgs e) {

        }

        private void VideoTimeTextBlock_SelectionChanged(object sender,RoutedEventArgs e) {

        }
    }
}
