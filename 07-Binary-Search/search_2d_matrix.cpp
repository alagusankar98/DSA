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