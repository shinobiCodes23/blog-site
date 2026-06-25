async function addBlog() {
    const title= document.getElementById("title").value;
    const tag= document.getElementById("tag").value;
    const text= document.getElementById("text").value;
    const response= await fetch("/addBlog", {
        method: "POST",
        headers: {"Content-Type":"application/json"},
        body: JSON.stringify({"title": title, "tag": tag, "text": text})
    });
    const data= await response.json();
    if (data.bool==true) {
        document.getElementById("output").innerHTML= "Added Successfully";
    }
    else {
        document.getElementById("output").innerHTML= "Failed to Add";
    }
    document.getElementById("title").value= "";
    document.getElementById("tag").value= "";
    document.getElementById("text").value= "";
}

function addBlogWindow() {
    window.location.href= "/add";
}

function editBlogWindow() {
    sessionStorage.setItem("title", document.getElementById("title").innerHTML);
    sessionStorage.setItem("content", document.getElementById("content").innerHTML);
    window.location.href= "/update";
}

async function editBlog() {
    const text= document.getElementById("Text").value;
    const response= await fetch("/updateBlog", {
        method: "POST",
        headers: {"Content-Type":"application/json"},
        body: JSON.stringify({"title": sessionStorage.getItem("title"), "text": text})
    });
    const data= await response.json();
    if (data.bool==true) {
        document.getElementById("Output").innerHTML= "Updated Successfully";
    }
    else {
        document.getElementById("Output").innerHTML= "Update Failed";
    }
    document.getElementById("Text").value= "";
}

async function broadSearch() {
    const response= await fetch("/broadSearch", {
        method: "POST",
        headers: {"Content-Type":"application/json"},
        body: JSON.stringify({"keyword": document.getElementById("search").value})
    });
    const data= await response.json();
    const blogss= document.getElementById("blogs");
    blogss.innerHTML= "";
    document.getElementById("search").value= "";
    if (data.blogs.length>0) {
        for (let i=0; i<data.blogs.length; i++) {
            blogss.innerHTML += `<a href="/readBlog?query=${encodeURIComponent(data.blogs[i])}">${data.blogs[i]}</a><br>`;
        }
    }
    else {
        blogss.innerHTML= "No Blogs Found";
    }
}

async function narrowSearch() {
    const response= await fetch("/narrowSearch", {
        method: "POST",
        headers: {"Content-Type":"application/json"},
        body: JSON.stringify({"title": document.getElementById("search").value})
    });
    const data= await response.json();
    const blogss= document.getElementById("blogs");
    blogss.innerHTML= "";
    document.getElementById("search").value= "";
    if (data.blogs!="") {
        blogss.innerHTML += `<a href="/readBlog?query=${encodeURIComponent(data.blogs[i])}">${data.blogs[i]}</a><br>`;
    }
    else {
        blogss.innerHTML= "No Blogs Found";
    }
}

window.onload= async()=>{
    const response= await fetch("/");
    const response2= await fetch("/renderBlogs");
    const data= await response2.json();
    const blogsDiv= document.getElementById("blogs");
    if (blogsDiv) {
        for (let i=0; i<data.blogs.length; i++) {
            blogsDiv.innerHTML += `<a href="/readBlog?query=${encodeURIComponent(data.blogs[i])}">${data.blogs[i]}</a><br>`;
        }
        const addButton= document.getElementById("add");
        if (addButton) {
            addButton.addEventListener("click", addBlogWindow);
        }
        const searchBtn1= document.getElementById("broadSearch");
        const searchBtn2= document.getElementById("narrowSearch");
        if (searchBtn1) {
            searchBtn1.addEventListener("click", broadSearch);
        }
        if (searchBtn2) {
            searchBtn2.addEventListener("click", narrowSearch);
        }
    }
    const saveButton= document.getElementById("Add");
    if (saveButton) {
        saveButton.addEventListener("click", addBlog);
    }
    const editButton= document.getElementById("edit");
    if (editButton) {
        editButton.addEventListener("click", editBlogWindow);
    }
    const updateButton= document.getElementById("update");
    if (updateButton) {
        document.getElementById("Text").value= sessionStorage.getItem("content");
        updateButton.addEventListener("click", editBlog);
    }
};