/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/

#define RLBOX_SINGLE_THREADED_INVOCATIONS
#define RLBOX_USE_STATIC_CALLS() rlbox_noop_sandbox_lookup_symbol

#include "intro/intro_start.h"

#include "lang/lang_keys.h"
#include "intro/intro_qr.h"
#include "intro/intro_phone.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "main/main_account.h"
#include "main/main_app_config.h"
#include "rlbox.hpp"
#include "rlbox_noop_sandbox.hpp"

using namespace rlbox;

namespace Intro {
namespace details {

// QString boobooString(int x){
// 	if (x < 10) {
// 		return "Something else";
// 	} else {
// 		return "Failed";
// 	}
// 	return "Here";
// }

// void echo(const char* x){
// 	printf("> mylib: %s\n", x);
// }

QString sandboxThis(int x){
	if (x == 9) {
		return "asdfasdf";
	} else{
		return "not here";
	}
}

int simpleAddTest(int a, int b)
{
  return a + b;
}

StartWidget::StartWidget(
	QWidget *parent,
	not_null<Main::Account*> account,
	not_null<Data*> data)
: Step(parent, account, data, true) {

	rlbox::rlbox_sandbox<rlbox_noop_sandbox> sandbox;
	sandbox.create_sandbox();
	setMouseTracking(true);

	// const char* helloStr = "hi hi!";
   	// size_t helloSize = strlen(helloStr) + 1;
   	// // allocate memory in the sandbox:
  	// auto taintedStr = sandbox.malloc_in_sandbox<char>(helloSize);
   	// // copy helloStr into the sandbox:
   	// std::strncpy(taintedStr.unverified_safe_pointer_because(helloSize, "writing to region"), helloStr, helloSize);
	// sandbox.invoke_sandbox_function(echo, taintedStr);
	// sandbox.free_in_sandbox(taintedStr);

	const int val1 = 20;
    const int val2 = 22;
    tainted<int, rlbox_noop_sandbox> a = val1;
    auto ret2 = sandbox.invoke_sandbox_function(simpleAddTest, a, val2);
	int r2 = ret2.UNSAFE_unverified();
	printf("%i", r2);

	// auto str1 = sandbox.invoke_sandbox_function(sandboxThis, 9).copy_and_verify([](T_Ret(*)(tainted<QString> ret)){
	// 	return ret;
	// });
	setTitleText(rpl::single(qsl("str1")));


	// QString title = boobooString(11);
	// setTitleText(rpl::single(title));
	sandbox.destroy_sandbox();
	setDescriptionText(tr::lng_intro_about());
	show();
}

void StartWidget::submit() {
	account().destroyStaleAuthorizationKeys();
	const auto qrLogin = account().appConfig().get<QString>(
		"qr_login_code",
		"[not-set]");
	DEBUG_LOG(("qr_login_code: %1").arg(qrLogin));
	if (qrLogin == "primary") {
		goNext<QrWidget>();
	} else {
		goNext<PhoneWidget>();
	}
}

rpl::producer<QString> StartWidget::nextButtonText() const {
	return tr::lng_start_msgs();
}

} // namespace details
} // namespace Intro
