

// /**
//  * @brief testing function to compare quality selection methods
//  *
//  * compares percentage of frames in stack for  laplacian vs gradient magnitude method  are the same
//  *
//  * @param frames
//  */

// void Limiter::compare_methods(std::vector<frameInfo> frames)
// {
//     size_t num_frames = frames.size();
//     std::vector<size_t> sorted_l(num_frames), sorted_g(num_frames);
//     std::vector<scoredFrame> scoredFramesL;
//     std::vector<scoredFrame> scoredFramesG;

//     FrameQuality gradient = FrameQuality(QualityMetric::GRADIENT);
//     FrameQuality laplacian = FrameQuality(QualityMetric::LAPLACIAN);
//     for (size_t i = 0; i < num_frames; i++)
//     {
//         scoredFrame new_frame{frames[i], gradient.score(frames[i])};
//         scoredFramesG.push_back(new_frame);
//         new_frame = {frames[i], laplacian.score(frames[i])};
//         scoredFramesL.push_back(new_frame);
//     }
//     std::iota(sorted_l.begin(), sorted_l.end(), 0);
//     std::sort(sorted_l.begin(), sorted_l.end(), [&](size_t a, size_t b)
//               { return scoredFramesL[a].score > scoredFramesL[b].score; });

//     std::iota(sorted_g.begin(), sorted_g.end(), 0);
//     std::sort(sorted_g.begin(), sorted_g.end(), [&](size_t a, size_t b)
//               { return scoredFramesG[a].score > scoredFramesG[b].score; });

//     getStackSame(sorted_g, sorted_l, limit_frames_num);
// }
