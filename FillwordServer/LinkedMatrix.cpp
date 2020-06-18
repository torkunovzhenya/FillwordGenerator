#include "LinkedMatrix.hpp"

Node::Node(int row, int col) : row(row), col(col) {}


LinkedMatrix::LinkedMatrix(int n, int m)
{
    rows = 0;
    cols = n * m;
    figures_count = std::vector<int>(n * m + 1);

    root = new Node(0, 0);
    Node* curr = root;

    col_heads.push_back(root);
    row_heads.push_back(root);

    for (int i = 1; i <= n * m; ++i)
    {
        curr->right = new Node(0, i);
        curr->right->left = curr;
        curr = curr->right;

        // Last in col
        curr->up = curr;
        curr->down = curr;

        // Adding to vector with links to columns' heads
        col_heads.push_back(curr);
    }
}


void LinkedMatrix::AddLocation(std::vector<int> cells)
{
    Node* row_head = row_heads[rows];

    // Adding new row to matrix
    row_head->down = new Node(++rows, 0);
    row_head = row_head->down;
    row_heads.push_back(row_head);

    Node* cur_node = row_head;
    for (int col : cells)
    {
        Node* col_head = col_heads[col];

        // Increasing number of figures in cell (column)
        figures_count[col]++;

        // Adding to row
        cur_node->right = new Node(rows, col);
        cur_node->right->left = cur_node;

        cur_node = cur_node->right;

        // Adding to col
        col_head->up->down = cur_node;
        cur_node->up = col_head->up;

        // Connect last node in column with column head
        col_head->up = cur_node;
        cur_node->down = col_head;
    }

    // Connect last node in row with row head
    cur_node->right = row_head;
    row_head->left = cur_node;
}

Node *LinkedMatrix::ChooseCell()
{
    int min_figures = -1;
    int min_cell = 0;

    // Searching for min nodes in column, -1 means that column must be skipped
    for (int i = 1; i <= cols; ++i)
        if (min_figures == -1 || (figures_count[i] != -1 && figures_count[i] < min_figures))
        {
            min_figures = figures_count[i];
            min_cell = i;
        }

    // Means that there no cells left (no columns)
    if (min_figures == -1)
        return nullptr;

    return col_heads[min_cell];
}

std::vector<Node*> LinkedMatrix::ChooseRow(int row)
{
    std::vector<Node*> rows_hidden;
    Node* row_head = row_heads[row];

    // Hiding all rows that have nodes in same (with chosen row) columns
    Node* node = row_head;
    while (node->right != row_head) // Cycle for nodes in chosen row
    {
        node = node->right;

        Node* same_col = node;
        while (same_col->down != node) // Cycle for nodes in same column
        {
            same_col = same_col->down;

            // If this node is column head skip it
            if (same_col->row == 0)
                continue;

            // Hide and append to array to restore in future
            Node* row_to_hide = row_heads[same_col->row];
            HideRow(row_to_hide);
            rows_hidden.push_back(row_to_hide);
        }
    }

    // Hide and append to array to restore in future
    HideRow(row_head);
    rows_hidden.push_back(row_head);

    // Hiding columns, we needn't to hide references
    node = row_head;
    while (node->right != row_head)
    {
        node = node->right;

        // Will be -1 after it
        figures_count[node->col]--;
    }

    return rows_hidden;
}


void LinkedMatrix::HideRow(Node* row)
{
    Node* node = row;
    while (node->right != row)
    {
        node = node->right;

        node->up->down = node->down;
        node->down->up = node->up;

        figures_count[node->col]--;
    }
}


void LinkedMatrix::RestoreRows(std::vector<Node*> rows_hidden)
{
    Node* main_row_head = rows_hidden.back();

    // Showing columns by making values not equal to -1
    Node* node = main_row_head;
    while (node->right != main_row_head)
    {
        node = node->right;
        figures_count[node->col] = 0;
    }

    // Showing rows in reversed order not to make mistakes by restoring references
    for (int i = rows_hidden.size() - 1; i >= 0; --i)
        ShowRow(rows_hidden[i]);
}


void LinkedMatrix::ShowRow(Node* row)
{
    Node* node = row;
    while (node->right != row)
    {
        node = node->right;

        node->up->down = node;
        node->down->up = node;

        figures_count[node->col]++;
    }
}


bool LinkedMatrix::isColumnEmpty(Node *col)
{
    return figures_count[col->col] == 0;
}


std::vector<int> LinkedMatrix::getFigureCells(int row)
{
    std::vector<int> cells;
    Node* row_head = row_heads[row];

    Node* node = row_head;
    while (node->right != row_head)
    {
        node = node->right;
        cells.push_back(node->col);
    }

    return cells;
}


LinkedMatrix::~LinkedMatrix()
{
    Node* row = root;
    while (row->down != nullptr)
    {
        row = row->down;

        Node* node = row->right;
        while (node->right != row)
        {
            node = node->right;
            delete node->left;
        }

        delete node;
    }

    for (int i = 1; i <= rows; ++i)
        delete row_heads[i];

    for (int i = 1; i <= cols; ++i)
        delete col_heads[i];

    delete root;
}
