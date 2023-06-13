#ifndef BASE_CALLBACK
#define BASE_CALLBACK

#include <functional>
#include <type_traits>
#include <memory>

// provide similar dev experience to chromium callback.
// ref: https://chromium.googlesource.com/chromium/src/+/master/docs/callback.md

namespace base {
	class ParamWrapper {
	public:
		ParamWrapper() :param(nullptr) {};

		void SetParam(void* param) {
			this->param = param;
		}

		template <class T>
		void SetDeleteFunc(T* type_param) {
			auto lambda_helper = [](void* param)->void
			{
				delete((T*)param);
			};
			delete_func = lambda_helper;
		}

		~ParamWrapper() {
			if (param) {
				delete_func(param);
				param = nullptr;
			}
		}
		void* param;
		std::function<void(void*)> delete_func;
	};

	template <typename Signature>
	class OnceCallback;

	class CallbackBase {
	public:
		virtual void* operate() { return nullptr; };
		virtual void setarg1(void* arg1) {};
	};

	template <class ReturnT, class ...Args>
	class OnceCallback<ReturnT(Args...)> :public CallbackBase {
	public:
		typedef std::function<ReturnT(Args...)> KeyFunc;
		constexpr OnceCallback() = default;
		OnceCallback(OnceCallback&& to) {
			to.param1 = this->param1;
			to.bindFunc = std::move(this->bindFunc);
			this->param1 = nullptr;
		}

		OnceCallback& operator=(OnceCallback&& input) noexcept {  // noexcept = default;
			if (&input == this)
				return *this;
			// Release any resource we're holding
			if (param1) {
				delete param1;
			}
			// Transfer ownership
			param1 = input.param1;
			input.param1 = nullptr;
			this->bindFunc = std::move(input.bindFunc);
			return *this;
		}

		OnceCallback<ReturnT(Args...)>(OnceCallback<ReturnT(Args...)>& other) {
			this->arg1 = other.arg1;
			this->bindFunc = other.bindFunc;
		}

		~OnceCallback() {
			if (param1)
				delete param1;
		}

		template <class ReturnT, class ResultT, class ...Args >
		OnceCallback(ReturnT(*func)(ResultT, Args ...), Args&&... args) {
			param1 = new ParamWrapper();
			ResultT* temp = nullptr;
			param1->SetDeleteFunc(temp);
			auto tempparam = param1;
			auto bind_func = std::bind(func, std::placeholders::_1, args...);
			auto lambda_helper = [bind_func, tempparam]()->ReturnT
			{
				ResultT* t = (ResultT*)(tempparam->param);
				return bind_func(*t);
			};
			bindFunc = lambda_helper;
		}

		template <class ReturnT, class ResultT, class Object, class ...Args >
		OnceCallback(ReturnT(Object::* func)(ResultT, Args ...), Object* obj_ptr, Args&&... args) {
			param1 = new ParamWrapper();
			ResultT* temp = nullptr;
			param1->SetDeleteFunc(temp);
			auto tempparam = param1;
			auto bind_func = std::bind(func, obj_ptr, std::placeholders::_1, args...);
			auto lambda_helper = [bind_func, tempparam]()->ReturnT
			{
				ResultT* t = (ResultT*)(tempparam->param);
				return bind_func(*t);
			};
			bindFunc = lambda_helper;
		}

		OnceCallback(ReturnT(*func)(Args ...), Args&&... args) {
			bindFunc = std::bind(func, args...);
		}

		template <class ReturnT, class Object, class ...Args >
		OnceCallback(ReturnT(Object::* func)(Args ...), Object* obj_ptr, Args&&... args) {
			bindFunc = std::bind(func, obj_ptr, args...);
		}

		void setarg1(void* arg1) {
			param1->SetParam(arg1);
		}

		template <class ReturnType>
		typename std::enable_if<std::is_same<void, ReturnType>::value, void*>::type operate_result() {
			Run();
			return nullptr;
		}

		template <class ReturnType>
		typename std::enable_if<!std::is_same<void, ReturnType>::value, void*>::type operate_result() {
			ReturnT* ret = new ReturnT(Run());
			return ret;
		}

		void* operate() {
			return operate_result<ReturnT>();
		}

		ReturnT Run() {
			return bindFunc();
		}

		template <class T >
		ReturnT Run(T args) {
			void* arg1 = (void*)&args;
			param1->SetParam(arg1);
			return bindFunc();
		}

	private:
		std::function<ReturnT(void)> bindFunc;
		ParamWrapper* param1 = nullptr;
	};

	class CallBackContainerBase {
	public:
		virtual void setarg1(void* arg1) {};
		virtual void* operate() { return nullptr; };
		virtual ~CallBackContainerBase() = default;
	};

	template <class ReturnT, class ... Args1>
	class CallBackContainer :public CallBackContainerBase {
	public:
		base::OnceCallback<ReturnT(Args1...)> callfunc;
		virtual void setarg1(void* arg1) {
			if (!!arg1) {
				callfunc.setarg1(arg1);
			}
		};
		virtual void* operate() { return callfunc.operate(); };
		CallBackContainer(base::OnceCallback<ReturnT(Args1...)>&& callback) {
			callfunc = std::move(callback);
		}
		virtual ~CallBackContainer() {};
	};

	template <class ReturnT, class ...Args >
	inline OnceCallback<ReturnT(Args...)> BindOnce(ReturnT(*func)(Args ...), Args&&... args) {
		return OnceCallback<ReturnT(Args...)>(func, std::forward<Args>(args)...);
	}

	template <class ReturnT, class ResultT, class ...Args >
	inline OnceCallback<ReturnT(ResultT, Args...)> BindOnce(ReturnT(*func)(ResultT, Args ...), Args&&... args) {
		return OnceCallback<ReturnT(ResultT, Args...)>(func, std::forward<Args>(args)...);
	}

	template<class ReturnT, class ClassT, class ... ClassFuncArgs>
	inline OnceCallback<ReturnT(ClassFuncArgs...)> BindOnce(ReturnT(ClassT::* func)(ClassFuncArgs ...), ClassT* obj_ptr, ClassFuncArgs&& ... args) {
		return OnceCallback<ReturnT(ClassFuncArgs...)>(func, obj_ptr, std::forward<ClassFuncArgs>(args)...);
	}

	template<class ReturnT, class ResultT, class ClassT, class ... ClassFuncArgs>
	inline OnceCallback<ReturnT(ResultT, ClassFuncArgs...)> BindOnce(ReturnT(ClassT::* func)(ResultT, ClassFuncArgs ...), ClassT* obj_ptr, ClassFuncArgs&& ... args) {
		return OnceCallback<ReturnT(ResultT, ClassFuncArgs...)>(func, obj_ptr, std::forward<ClassFuncArgs>(args)...);
	}

	//todo(Haichao): support more args for delayed params: callback.run(arg1,arg2,...)
}

#endif