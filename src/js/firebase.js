const firebaseConfig = {
  apiKey: "AIzaSyAvVAvABJNxy8GxZbg_bzRNCvlAIvWGON0",
  authDomain: "hethongnhathongminh.firebaseapp.com",
  databaseURL: "https://hethongnhathongminh-default-rtdb.firebaseio.com",
  projectId: "hethongnhathongminh",
  storageBucket: "hethongnhathongminh.firebasestorage.app",
  messagingSenderId: "492237635095",
  appId: "1:492237635095:web:9482fd0660c72fb670e652",
};

firebase.initializeApp(firebaseConfig);


// Kiểm tra trạng thái đăng nhập khi tải lại trang
firebase.auth().onAuthStateChanged(function (user) {
  if (user) {
    document.getElementById("user-name").textContent = user.displayName;
    document.getElementById("user-info").style.display = "inline-block";
    document.getElementById("logout-btn").style.display = "inline-block";
    document.getElementById("login-btn").style.display = "none";
    document.getElementById("signup-btn").style.display = "none";
  } else {
    document.getElementById("user-info").style.display = "none";
    document.getElementById("logout-btn").style.display = "none";
    document.getElementById("login-btn").style.display = "inline-block";
    document.getElementById("signup-btn").style.display = "inline-block";
  }
});