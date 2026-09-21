bool searchMatrix(const std::vector<std::vector<int>>& matrix, int target) {
    if(matrix.empty()) return false;
    const int rows = static_cast<int>(matrix.size());
    const int cols = static_cast<int>(matrix[0].size());

    int matchRow = -1;
    int upper = 0;
    int lower = rows - 1;

    // Find row
    while(upper <= lower){
        int mid = upper + (lower - upper) / 2;

        if(matrix[mid][0] <= target && matrix[mid][cols - 1] >= target){
            matchRow = mid;
            break;
        } else if(matrix[mid][0] > target){
            // Discard lower half
            lower = mid - 1;
        } else {
            // Discard upper half
            upper = mid + 1;
        }
    }

    if(matchRow == -1) return false; // No matching row found

    // Find element match
    upper = 0;
    lower = cols - 1;

    while(upper <= lower){
        int mid = upper + (lower - upper) / 2;

        if(matrix[matchRow][mid] == target){
            return true;
        } else if(matrix[matchRow][mid] > target){
            // Discard lower half
            lower = mid - 1;
        } else {
            // Discard upper half
            upper = mid + 1;
        }
    }

    return false;
}