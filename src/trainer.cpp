#include <mlpack/core.hpp>
#include <mlpack/methods/linear_svm/linear_svm.hpp>

using namespace mlpack;

int main()
{
    // Load the training data.
    arma::mat train_data;
    data::Load("/home/ubuntu/git-repos/autonomus_driver/src/helpers/normalized_data.csv", train_data);

    // Load the training labels.
    arma::Row<size_t> labels;
    data::Load("/home/ubuntu/git-repos/autonomus_driver/frames/training_data/labels.csv", labels);

    const size_t inputSize = train_data.n_rows; 
    const size_t numClasses = 3; 
    const double lambda = 0.01; 
    const double delta = 1.0; 

    LinearSVM<> lsvm(train_data, labels, numClasses, lambda, delta);
    mlpack::data::Save("linear_svm_model.bin", "linear_svm", lsvm);

    // arma::mat test_data;
    // data::Load("test_data.csv", test_data);

    // arma::Row<size_t> predictions;
    // lsvm.Classify(test_data, predictions);

    // data::Save("predictions.csv", predictions);

    return 0;
}