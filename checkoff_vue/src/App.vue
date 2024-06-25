<template>
  <HelloWorld :todo-items="todoItems"/>
</template>

<script>
import HelloWorld from './components/HelloWorld.vue'
import axios from 'axios';

export default {
  name: 'App',
  components: {
    HelloWorld
  },
  data() {
    return {
      todoItems: null
    }
  },
  mounted() {
    axios
      .get('http://localhost:3001/todo-item')
      .then(response => {
        this.todoItems = response.data.data;
        console.log("todoItems = " + JSON.stringify(this.todoItems));
      })
  }
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
}
</style>
