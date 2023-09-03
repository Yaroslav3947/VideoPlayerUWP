using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using VideoPlayerWrapper;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
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

            videoSlider.ValueChanged += OnSliderMoved;
            videoSlider.PointerPressed += OnSliderPressed;
            videoSlider.PointerReleased += OnSliderReleased;
        }

        private void OnSliderReleased(object sender,PointerRoutedEventArgs e) {
            Debug.WriteLine("Slider released");
            if(videoPlayer.GetIsPaused()) {
                videoPlayer.PlayPauseVideo();
                ////TODO: change the icon to pause
            }
        }

        private void OnSliderPressed(object sender,PointerRoutedEventArgs e) {
            Debug.WriteLine("Slider pressed");
            if(!videoPlayer.GetIsPaused()) {
                videoPlayer.PlayPauseVideo();
            }
        }

        private void OnSliderMoved(object sender,RangeBaseValueChangedEventArgs e) {
            long sliderValue = (long)e.NewValue;

            //Debug.WriteLine("Slider moved" + sliderValue);

            videoPlayer.SetPosition(sliderValue * 100);
        }
        private void OnPositionChanged(long newPosition) {
            videoSlider.Value = newPosition;

            UpdateDurationInfo(newPosition * 100);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            base.OnNavigatedTo(e);
            //videoPlayer.OpenURL("VideoMusk30fps.mp4");
            videoPlayer.OpenURL("SampleVideo25fps.mp4");

            long maxSliderValue = videoPlayer.GetDuration() / 100;

            videoSlider.Minimum = 0;
            videoSlider.Maximum = maxSliderValue;

            //UpdateDurationInfo(20000000);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e) {
        }

        private void Window_SizeChanged(object sender,WindowSizeChangedEventArgs e) {
            double newWidth = videoGrid.ActualWidth;
            double newHeight = videoGrid.ActualHeight;

            videoPlayer.ResizeSwapChainPanel(newWidth,newHeight);
        }

        private void PlayPauseButton_Click(object sender,RoutedEventArgs e) {
            videoPlayer.PlayPauseVideo();
        }

        private void MuteButton_Click_(object sender,RoutedEventArgs e) {
            if(videoPlayer.GetIsMuted()) {
                videoPlayer.Unmute();
                ////TODO: change the icon
            } else {
                videoPlayer.Mute();
                ////TODO: change the icon
            }
        }

        public void UpdateDurationInfo(long currentPosition) {
            long duration = videoPlayer.GetDuration();

            TimeSpan currentTime = TimeSpan.FromMilliseconds(currentPosition / 10000);
            TimeSpan totalTime = TimeSpan.FromMilliseconds(duration / 10000);

            string format = "hh\\:mm\\:ss";

            string currentTimeStr = currentTime.ToString(format);
            string totalTimeStr = totalTime.ToString(format);

            string tStr = currentTimeStr + " / " + totalTimeStr;

            VideoTimeTextBlock.Text = tStr;
        }

        private async void OpenMenuItem_Click(object sender,RoutedEventArgs e) {
            FileOpenPicker filePicker = new FileOpenPicker();
            filePicker.ViewMode = PickerViewMode.List;
            filePicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            filePicker.FileTypeFilter.Add(".mp4");

            StorageFile file = await filePicker.PickSingleFileAsync();
            if(file != null) {
                string selectedFilePath = file.Path;
            }

        }
    }
}
