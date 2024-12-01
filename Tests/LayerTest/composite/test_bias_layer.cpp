#include <MetaNN/meta_nn.h>
#include <calculate_tags.h>
#include <data_gen.h>
#include <cassert>
#include <iostream>
using namespace MetaNN;
using namespace std;

namespace {
	using CommonInputMap = LayerInMap<LayerKV<LayerInput, Matrix<CheckElement, CheckDevice>>>;

	void test_bias_layer1() {
		cout << "Test bias layer case 1...\t" << flush;
		using RootLayer = MakeInferLayer<BiasLayer, PParamTypeIs<Matrix<CheckElement, CheckDevice>>>;
		static_assert(!RootLayer::IsUpdate);
		static_assert(!RootLayer::IsFeedbackOutput);

		RootLayer layer("root", 2, 1);

		//Initialization
		auto filler = MakeInitializer<CheckElement>();
		LoadBuffer<CheckElement, CheckDevice> loadBuffer;

		auto mat = GenTensor<CheckElement>(0, 1, 2, 1);
		filler.SetParam("root/param", mat);

		layer.Init(filler, loadBuffer);

		auto input = GenTensor<CheckElement>(0.5f, -0.1f, 2, 1);
		auto bi = LayerInputCont<RootLayer>().Set<LayerInput>(input);

		LayerNeutralInvariant(layer);
		auto out = layer.FeedForward(bi);
		auto res = Evaluate(out.Get<LayerOutput>());
		assert(fabs(res(0, 0) - input(0, 0) - mat(0, 0)) < 0.001);
		assert(fabs(res(1, 0) - input(1, 0) - mat(1, 0)) < 0.001);

		auto out_grad = layer.FeedBackward(LayerOutputCont<RootLayer>());

		loadBuffer.Clear();
		layer.SaveWeights(loadBuffer);
		auto* w = loadBuffer.TryGet<CategoryTags::Tensor<2>>("root/param");
		assert(w);

		auto wInfo = *w;

		assert(wInfo.Shape() == mat.Shape());
		for (size_t i = 0; i < 2; ++i)
		{
			for (size_t j = 0; j < 1; ++j)
			{
				assert(fabs(wInfo(i, j) - mat(i, j)) < 0.001f);
			}
		}

		LayerNeutralInvariant(layer);
		cout << "done" << endl;
	}

}


namespace Test::Layer::Composite
{
	void test_bias_layer()
	{
		test_bias_layer1();
	}
}