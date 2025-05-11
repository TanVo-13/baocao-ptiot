// Hiển thị/ẩn modal
const loginModal = document.getElementById("login-modal");
const signupModal = document.getElementById("signup-modal");


document.getElementById("login-btn").onclick = () =>
  (loginModal.style.display = "block");
document.getElementById("signup-btn").onclick = () =>
  (signupModal.style.display = "block");

document.querySelectorAll(".close").forEach((btn) => {
  btn.onclick = () => {
    loginModal.style.display = "none";
    signupModal.style.display = "none";
  };
});

window.onclick = (event) => {
  if (event.target == loginModal) loginModal.style.display = "none";
  if (event.target == signupModal) signupModal.style.display = "none";
};

// Đăng ký
document
  .getElementById("signup-form")
  .addEventListener("submit", function (e) {
    e.preventDefault();
    const name = document.getElementById("signup-name").value;
    const email = document.getElementById("signup-email").value;
    const password = document.getElementById("signup-password").value;
    const inputCode = document.getElementById("verification-code").value;

    // Kiểm tra mã xác nhận
    if (!generatedCode || inputCode != generatedCode) {
      alert("Mã xác nhận không đúng hoặc chưa được gửi.");
      return;
    }

    // Kiểm tra xem email đã tồn tại chưa
    firebase
      .auth()
      .fetchSignInMethodsForEmail(email)
      .then((methods) => {
        if (methods.length > 0) {
          alert(
            "Email đã tồn tại. Vui lòng đăng nhập hoặc dùng email khác."
          );
          throw new Error("Email đã tồn tại");
        } else {
          // Email chưa tồn tại → tạo tài khoản
          return firebase
            .auth()
            .createUserWithEmailAndPassword(email, password);
        }
      })
      .then((userCredential) => {
        if (userCredential) {
          return userCredential.user.updateProfile({
            displayName: name,
          });
        }
      })
      .then(() => {
        alert("Đăng ký thành công!");
        signupModal.style.display = "none";
      })
      .catch((error) => {
        if (error.message !== "Email đã tồn tại") {
          alert("Lỗi: " + error.message);
        }
      });
  });

// Đăng nhập
document
  .getElementById("login-form")
  .addEventListener("submit", function (e) {
    e.preventDefault();
    const email = document.getElementById("login-email").value;
    const password = document.getElementById("login-password").value;

    firebase
      .auth()
      .signInWithEmailAndPassword(email, password)
      .then((userCredential) => {
        alert("Đăng nhập thành công!");
        const user = userCredential.user;
        loginModal.style.display = "none";

        document.getElementById("user-name").textContent =
          user.displayName; // Hiển thị tên
        document.getElementById("user-info").style.display =
          "inline-block"; // Hiển thị thông tin người dùng

        document.getElementById("logout-btn").style.display =
          "inline-block";
        document.getElementById("login-btn").style.display = "none";
        document.getElementById("signup-btn").style.display = "none";
      })
      .catch((error) => {
        alert("Lỗi: " + error.message);
      });
  });

// Đăng xuất
document
  .getElementById("logout-btn")
  .addEventListener("click", function () {
    firebase
      .auth()
      .signOut()
      .then(() => {
        alert("Đã đăng xuất!");
        document.getElementById("logout-btn").style.display = "none";
        document.getElementById("login-btn").style.display =
          "inline-block";
        document.getElementById("signup-btn").style.display =
          "inline-block";
      });
  });


let generatedCode = null;

document
  .getElementById("send-code-btn")
  .addEventListener("click", function () {
    const email = document.getElementById("signup-email").value;
    if (!email) {
      alert("Vui lòng nhập email trước khi gửi mã.");
      return;
    }

    generatedCode = Math.floor(100000 + Math.random() * 900000); // Mã 6 chữ số

    // Gửi mã qua Firebase hoặc email server của bạn
    fetch("http://localhost:3000/send-verification-code", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ email: email, code: generatedCode }),
    })
      .then((res) => res.json())
      .then((data) => {
        if (data.success) {
          alert("Đã gửi mã xác nhận đến email!");
        } else {
          alert("Không thể gửi mã. Vui lòng thử lại.");
        }
      })
      .catch((err) => {
        console.error(err);
        alert("Lỗi khi gửi mã xác nhận.");
      });
  });