#pragma once

#include <functional>

#include "FN_multi_function.h"

namespace FN {

class MF_CombineColor final : public MultiFunction {
 public:
  MF_CombineColor();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_SeparateColor final : public MultiFunction {
 public:
  MF_SeparateColor();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_CombineVector final : public MultiFunction {
 public:
  MF_CombineVector();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_SeparateVector final : public MultiFunction {
 public:
  MF_SeparateVector();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_FloatArraySum final : public MultiFunction {
 public:
  MF_FloatArraySum();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_FloatRange final : public MultiFunction {
 public:
  MF_FloatRange();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_ObjectWorldLocation final : public MultiFunction {
 public:
  MF_ObjectWorldLocation();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_ObjectVertexPositions final : public MultiFunction {
 public:
  MF_ObjectVertexPositions();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_TextLength final : public MultiFunction {
 public:
  MF_TextLength();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_RandomFloat final : public MultiFunction {
 public:
  MF_RandomFloat();
  void call(MFMask mask, MFParams parms, MFContext context) const override;
};

template<typename T> class MF_ConstantValue : public MultiFunction {
 private:
  T m_value;

 public:
  MF_ConstantValue(T value) : m_value(std::move(value))
  {
    MFSignatureBuilder signature("Constant " + CPP_TYPE<T>().name());
    signature.single_output<T>("Output");
    this->set_signature(signature);
  }

  void call(MFMask mask, MFParams params, MFContext UNUSED(context)) const override
  {
    MutableArrayRef<T> output = params.uninitialized_single_output<T>(0, "Output");

    mask.foreach_index([&](uint i) { new (output.begin() + i) T(m_value); });
  }
};

template<typename FromT, typename ToT> class MF_Convert : public MultiFunction {
 public:
  MF_Convert()
  {
    MFSignatureBuilder signature(CPP_TYPE<FromT>().name() + " to " + CPP_TYPE<ToT>().name());
    signature.single_input<FromT>("Input");
    signature.single_output<ToT>("Output");
    this->set_signature(signature);
  }

  void call(MFMask mask, MFParams params, MFContext UNUSED(context)) const override
  {
    VirtualListRef<FromT> inputs = params.readonly_single_input<FromT>(0, "Input");
    MutableArrayRef<ToT> outputs = params.uninitialized_single_output<ToT>(1, "Output");

    for (uint i : mask.indices()) {
      const FromT &from_value = inputs[i];
      new (outputs.begin() + i) ToT(from_value);
    }
  }
};

class MF_SimpleVectorize final : public MultiFunction {
 private:
  const MultiFunction &m_function;
  Vector<bool> m_input_is_vectorized;
  Vector<uint> m_vectorized_inputs;
  Vector<uint> m_output_indices;

 public:
  MF_SimpleVectorize(const MultiFunction &function, ArrayRef<bool> input_is_vectorized);
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_ContextVertexPosition final : public MultiFunction {
 public:
  MF_ContextVertexPosition();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_ContextCurrentFrame final : public MultiFunction {
 public:
  MF_ContextCurrentFrame();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_SwitchSingle final : public MultiFunction {
 private:
  const CPPType &m_type;

 public:
  MF_SwitchSingle(const CPPType &type);
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_SwitchVector final : public MultiFunction {
 private:
  const CPPType &m_type;

 public:
  MF_SwitchVector(const CPPType &type);
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_PerlinNoise final : public MultiFunction {
 public:
  MF_PerlinNoise();
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_MapRange final : public MultiFunction {
 private:
  bool m_clamp;

 public:
  MF_MapRange(bool clamp);
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

class MF_Clamp final : public MultiFunction {
 private:
  bool m_sort_minmax;

 public:
  MF_Clamp(bool sort_minmax);
  void call(MFMask mask, MFParams params, MFContext context) const override;
};

template<typename InT, typename OutT> class MF_Custom_In1_Out1 final : public MultiFunction {
 private:
  using FunctionT = std::function<void(MFMask mask, VirtualListRef<InT>, MutableArrayRef<OutT>)>;
  FunctionT m_fn;

 public:
  MF_Custom_In1_Out1(StringRef name, FunctionT fn) : m_fn(std::move(fn))
  {
    MFSignatureBuilder signature(name);
    signature.single_input<InT>("Input");
    signature.single_output<OutT>("Output");
    this->set_signature(signature);
  }

  void call(MFMask mask, MFParams params, MFContext UNUSED(context)) const override
  {
    VirtualListRef<InT> inputs = params.readonly_single_input<InT>(0);
    MutableArrayRef<OutT> outputs = params.uninitialized_single_output<OutT>(1);
    m_fn(mask, inputs, outputs);
  }
};

template<typename InT1, typename InT2, typename OutT>
class MF_Custom_In2_Out1 final : public MultiFunction {
 private:
  using FunctionT = std::function<void(
      MFMask mask, VirtualListRef<InT1>, VirtualListRef<InT2>, MutableArrayRef<OutT>)>;

  FunctionT m_fn;

 public:
  MF_Custom_In2_Out1(StringRef name, FunctionT fn) : m_fn(std::move(fn))
  {
    MFSignatureBuilder signature(name);
    signature.single_input<InT1>("Input 1");
    signature.single_input<InT2>("Input 2");
    signature.single_output<OutT>("Output");
    this->set_signature(signature);
  }

  void call(MFMask mask, MFParams params, MFContext UNUSED(context)) const override
  {
    VirtualListRef<InT1> inputs1 = params.readonly_single_input<InT1>(0);
    VirtualListRef<InT2> inputs2 = params.readonly_single_input<InT2>(1);
    MutableArrayRef<OutT> outputs = params.uninitialized_single_output<OutT>(2);
    m_fn(mask, inputs1, inputs2, outputs);
  }
};

template<typename T> class MF_VariadicMath final : public MultiFunction {
 private:
  using FunctionT =
      std::function<void(MFMask mask, VirtualListRef<T>, VirtualListRef<T>, MutableArrayRef<T>)>;

  uint m_input_amount;
  FunctionT m_fn;

 public:
  MF_VariadicMath(StringRef name, uint input_amount, FunctionT fn)
      : m_input_amount(input_amount), m_fn(fn)
  {
    BLI_STATIC_ASSERT(std::is_trivial<T>::value, "");
    BLI_assert(input_amount >= 1);
    MFSignatureBuilder signature(name);
    for (uint i = 0; i < m_input_amount; i++) {
      signature.single_input<T>("Input");
    }
    signature.single_output<T>("Output");
    this->set_signature(signature);
  }

  void call(MFMask mask, MFParams params, MFContext UNUSED(context)) const override
  {
    MutableArrayRef<T> outputs = params.uninitialized_single_output<T>(m_input_amount, "Output");

    if (m_input_amount == 1) {
      VirtualListRef<T> inputs = params.readonly_single_input<T>(0, "Input");
      for (uint i : mask.indices()) {
        outputs[i] = inputs[i];
      }
    }
    else {
      BLI_assert(m_input_amount >= 2);
      VirtualListRef<T> inputs0 = params.readonly_single_input<T>(0, "Input");
      VirtualListRef<T> inputs1 = params.readonly_single_input<T>(1, "Input");
      m_fn(mask, inputs0, inputs1, outputs);

      for (uint param_index = 2; param_index < m_input_amount; param_index++) {
        VirtualListRef<T> inputs = params.readonly_single_input<T>(param_index, "Input");
        m_fn(mask, VirtualListRef<T>::FromFullArray(outputs), inputs, outputs);
      }
    }
  }
};

}  // namespace FN
