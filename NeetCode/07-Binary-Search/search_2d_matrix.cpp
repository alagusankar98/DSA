bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
if(matrix.empty() || matrix[0].empty()) return false;
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());

    int matchRow = -1;
    int top = 0;
    int bottom = rows - 1;

    // Find row
    while(top <= bottom){
        int mid = top + (bottom - top) / 2;

        if(matrix[mid][0] <= target && matrix[mid][cols - 1] >= target){
            matchRow = mid;
            break;
        } else if(matrix[mid][0] > target){
            // Discard bottom half
            bottom = mid - 1;
        } else {
            // Discard top half
            top = mid + 1;
        }
    }

    if(matchRow == -1) return false; // No matching row found

    // Find element match
    top = 0;
    bottom = cols - 1;

    while(top <= bottom){
        int mid = top + (bottom - top) / 2;

        if(matrix[matchRow][mid] == target){
            return true;
        } else if(matrix[matchRow][mid] > target){
            // Discard bottom half
            bottom = mid - 1;
        } else {
            // Discard top half
            top = mid + 1;
        }
    }

    return false;
}

// Optimal one pass solution, but prefer the two pass solution as it is faster due to absence of multiple integer division

// bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
//     if(matrix.empty() || matrix[0].empty()) return false;
//     const int rows = static_cast<int>(matrix.size());
//     const int cols = static_cast<int>(matrix[0].size());

//     int64_t left = 0;
//     int64_t right = static_cast<int64_t>(rows * cols) - 1;

//     while(left <= right){
//         int64_t mid = left + (right - left) / 2;

//         // Convert mid back to rows and columns
//         int row = static_cast<int>(mid / cols);
//         int column = static_cast<int>(mid % cols);

//         if(matrix[row][column] == target){
//             return true;
//         } else if (matrix[row][column] > target){
//             right = mid - 1;
//         } else {
//             left = mid + 1;
//         }
//     }

//     return false;
// }