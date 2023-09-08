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
using Windows.Storage.Streams;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
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
            ConnectSignals();

            controlPanel.Visibility = Visibility.Collapsed; // hide the control panel
        }

        private void ConnectSignals() {
            Window.Current.SizeChanged += WindowSizeChanged;

            videoPlayer.VideoPlayerPositionChanged += OnVideoPlayerPositionChanged;
            videoPlayer.VideoPlayerEndOfStream += OnVideoPlayerEndOfStream;

            ConnectSliderSignals();
        }

        private void ConnectSliderSignals() {
            videoSlider.ValueChanged += OnSliderMoved;
        }

        private void OnVideoPlayerEndOfStream(VideoPlayerWrap sender) {
            // TODO: change the icon to play
            videoPlayer.SetPosition(0);
            videoPlayer.PlayPauseVideo();
            //videoSlider.Value = 0;
        }

        private void OnVideoPlayerPositionChanged(VideoPlayerWrap sender,long newVideoPlayerPosition) {
            Dispatcher.RunAsync(CoreDispatcherPriority.Normal,() => {
                //videoSlider.Value = newVideoPlayerPosition;
                UpdateDurationInfo(newVideoPlayerPosition * 100);
            });

            ////TODO: work out updating the slider

        }
        private void OnSliderMoved(object sender,RangeBaseValueChangedEventArgs e) {
            long newSliderValue = (long)e.NewValue;

            videoPlayer.SetPosition(newSliderValue * 100);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e) {
            base.OnNavigatedTo(e);

            ////TODO: work out resizing the video player according to the window size
            videoPlayer.ResizeSwapChainPanel(1280,720,false);

        }

        private void SetSlider() {

            long maxSliderValue = videoPlayer.GetDuration() / 100;
            Debug.WriteLine("Max slider value: " + maxSliderValue);

            videoSlider.Minimum = 0;
            videoSlider.Maximum = maxSliderValue;
        }

        private void WindowSizeChanged(object sender,WindowSizeChangedEventArgs e) {
            double newVideoGridWidth = videoGrid.ActualWidth;
            double newVideoGridHeight = videoGrid.ActualHeight;

            if(newVideoGridHeight > 0 && newVideoGridWidth > 0) {
                if(!videoPlayer.GetIsPaused()) {
                    videoPlayer?.ResizeSwapChainPanel((int)newVideoGridWidth,(int)newVideoGridHeight,false);
                } else {
                    videoPlayer?.ResizeSwapChainPanel((int)newVideoGridWidth,(int)newVideoGridHeight,true);
                }
            }
        }

        private void PlayPauseButton_Click(object sender,RoutedEventArgs e) {
            videoPlayer.PlayPauseVideo();
            ////TODO: change the icon
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
            long videoDuration = videoPlayer.GetDuration();

            TimeSpan currentTime = TimeSpan.FromMilliseconds(currentPosition / 10000);
            TimeSpan totalTime = TimeSpan.FromMilliseconds(videoDuration / 10000);

            string format = "hh\\:mm\\:ss";

            string currentTimeStr = currentTime.ToString(format);
            string totalTimeStr = totalTime.ToString(format);

            string resultString = currentTimeStr + " / " + totalTimeStr;

            VideoTimeTextBlock.Text = resultString;
        }

        private async void OpenMenuItem_Click(object sender,RoutedEventArgs e) {
            FileOpenPicker filePicker = new FileOpenPicker();
            filePicker.ViewMode = PickerViewMode.List;
            filePicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            filePicker.FileTypeFilter.Add(".mp4");

            try {
                StorageFile file = await filePicker.PickSingleFileAsync();
                if(file != null) {
                    // Read the file as a byte stream.
                    IBuffer buffer = await FileIO.ReadBufferAsync(file);
                    byte[] fileBytes = buffer.ToArray();

                    videoPlayer.OpenURL(fileBytes,fileBytes.Length);
                    SetSlider();



                    controlPanel.Visibility = Visibility.Visible;
                }
            }
            catch(Exception ex) {
                Debug.WriteLine($"Error opening file: {ex.Message}");
            }
        }
    }
}
